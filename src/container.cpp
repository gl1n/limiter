#include <cgroup.h>
#include <container.h>
#include <cstring>
#include <fcntl.h>
#include <fstream>
#include <semaphore.h>
#include <sys/stat.h>
#include <sys/syscall.h>

static constexpr int STACK_SIZE = 1024 * 1024;
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

  std::cout << putOld << std::endl;
  //修改当前工作目录
  if (chdir("/") == -1) {
    perror("chdir error: ");
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

  if (mount("proc", "/proc", "proc", MS_NODEV | MS_NOSUID | MS_NOEXEC, NULL) ==
      -1) {
    perror("mount error: ");
    exit(EXIT_FAILURE);
  }

  //要在mount proc之后change_root，否则报错dev busy
  std::string new_root(_args->image);
  change_root(new_root);

  if (_args->hostname) {
    if (sethostname(_args->hostname, strlen(_args->hostname)) == -1) {
      perror("sethostname error: ");
    }
  }

  sem_t *sem = sem_open("/semaphore", 0); /* Open a preexisting semaphore. */
  /* sem_wait(sem);
  std::cout << "here" << std::endl;
  sem_close(sem);
  sem_unlink("/semaphore"); */
  execvp(_args->job[0], _args->job);
  perror("exec error: ");
  return 0;
}

void Container::run(ArgParser::Args *args) {

  sem_t *sem =
      sem_open("/semaphore", O_CREAT, 0644, 0); /* Initial value is 0. */

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

  sem_post(sem);
  sem_close(sem);

  waitpid(pid, NULL, 0);

  delete[] child_stack;
}
