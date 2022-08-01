#include <cgroup.h>
#include <container.h>
#include <cstring>
#include <fcntl.h>
#include <fstream>
#include <sys/stat.h>
#include <sys/syscall.h>

static constexpr int STACK_SIZE = 1024 * 1024;

//处理镜像
static void change_root(std::string const &new_root) {
  // bind mount镜像
  if (mount(new_root.data(), new_root.data(), "bind", MS_BIND | MS_REC, NULL) ==
      -1) {
    perror("mount error: ");
    exit(EXIT_FAILURE);
  }

  //创建一个目录用来挂在当前rootfs
  std::string putOld = new_root + "/.put_old";
  if (mkdir(putOld.data(), 0777) == -1) {
    perror("create .put_old error: ");
    exit(EXIT_FAILURE);
  }

  // pivot_root没有glibc封装，只能通过syscall调用
  // 将当前rootfs挂在到putOld, new_root挂载到/
  if (syscall(SYS_pivot_root, new_root.data(), putOld.data()) == -1) {
    perror("pivot_root error: ");
    exit(EXIT_FAILURE);
  }

  //修改当前工作目录
  if (chdir("/") == -1) {
    perror("chdir error: ");
    exit(EXIT_FAILURE);
  }

  // mount proc
  if (mount("proc", "/proc", "proc", MS_NODEV | MS_NOSUID | MS_NOEXEC, NULL) ==
      -1) {
    perror("mount error: ");
    exit(EXIT_FAILURE);
  }

  if (umount2("/.put_old", MNT_DETACH) == -1) {
    perror("umount2 error: ");
    exit(EXIT_FAILURE);
  }

  if (rmdir("/.put_old") == -1) {
    perror("rmdir error: ");
    exit(EXIT_FAILURE);
  }
}

static int call_back(void *args) {
  ArgParser::Args *_args = (ArgParser::Args *)args;

  //关掉write end，这样当另一个进程也关闭write end时，从read end读取会得到EOF
  close(_args->pipe_fd[1]);

  //阻塞等待EOF(父进程准备工作完成)
  char ch;
  if (read(_args->pipe_fd[0], &ch, 1) != 0) {
    std::cerr << "expected EOF" << std::endl;
    exit(EXIT_FAILURE);
  }

  std::string new_root(_args->image);
  change_root(new_root);

  //设置hostname
  if (_args->hostname) {
    if (sethostname(_args->hostname, strlen(_args->hostname)) == -1) {
      perror("sethostname error: ");
    }
  } else {
    if (sethostname("container", 9) == -1) {
      perror("sethostname error: ");
    }
  }

  execvp(_args->command[0], _args->command);
  perror("exec error: ");
  return 0;
}

void Container::run(ArgParser::Args *args) {

  if (pipe(args->pipe_fd) == -1) {
    perror("pipe error: ");
  }

  char *child_stack = new char[STACK_SIZE];

  int flags = SIGCHLD | CLONE_NEWUTS | CLONE_NEWPID | CLONE_NEWNS |
              CLONE_NEWNET | CLONE_NEWIPC | CLONE_NEWUSER;
  int pid = clone(call_back, child_stack + STACK_SIZE, flags,
                  (void *)args); //创建子线程
  //资源限制
  Cgroup cg(pid);
  cg.LimitCPU(args->cpu_period, args->cpu_quota);
  cg.LimitMem(args->memory, args->swap);

  //设置user namespace映射
  std::ofstream uid_map("/proc/" + std::to_string(pid) + "/uid_map",
                        std::ios::trunc);
  std::ofstream gid_map("/proc/" + std::to_string(pid) + "/gid_map",
                        std::ios::trunc);
  auto uid = getuid();
  std::string con = "0 " + std::to_string(static_cast<int>(uid)) + " 1";
  uid_map << con;
  uid_map.close();
  gid_map << con;
  gid_map.close();

  close(args->pipe_fd[0]); // read end用不到，直接关闭

  //关闭write end。当所有write end的文件描述符都被关闭时，尝试从read
  // end读取会得到EOF
  close(args->pipe_fd[1]); //通知子进程

  waitpid(pid, NULL, 0);

  delete[] child_stack;
}
