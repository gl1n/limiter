#include <container.h>
#include <cstring>

static constexpr int STACK_SIZE = 1024 * 1024;
static int call_back(void *args) {
  ArgParser::Args *_args = (ArgParser::Args *)args;
  if (mount("proc", "/proc", "proc", MS_NODEV | MS_NOSUID | MS_NOEXEC, NULL)) {
    std::cerr << "mount error" << std::endl;
    exit(EXIT_FAILURE);
  }
  if (_args->hostname) {
    sethostname(_args->hostname, strlen(_args->hostname));
  }
  execvp(_args->job[0], _args->job);
  std::cerr << "exec error" << std::endl;
  return 0;
}

void Container::run(ArgParser::Args *args) {
  char *child_stack = new char[STACK_SIZE];

  int flags = SIGCHLD | CLONE_NEWUTS | CLONE_NEWPID | CLONE_NEWNS |
              CLONE_NEWNET | CLONE_NEWIPC | CLONE_NEWUSER;
  int pid = clone(call_back, child_stack + STACK_SIZE, flags,
                  (void *)args); //创建子线程
  waitpid(pid, NULL, 0);
  delete[] child_stack;
}
