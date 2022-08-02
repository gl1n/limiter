#pragma once
#include <cgroup.h>

namespace ArgParser {

struct Args {
  //真正执行的命令
  char **command;

  char *hostname;
  // cpu配额
  int cpu_quota;
  // cpu配额基数
  int cpu_period;
  //最大可使用内存
  int memory;
  //最大可使用swap
  int swap;

  //镜像地址
  char *image;

  //用于进程同步
  int pipe_fd[2];

  Args()
      : command(nullptr), hostname(nullptr), cpu_quota(0), cpu_period(0),
        memory(-1), swap(-1), image(nullptr) {}

  ~Args() {
    if (command) {
      for (char **p = command; *p; p++) {
        delete[] * p;
      }
      delete[] command;
    }

    delete[] hostname;
    delete[] image;
  };
};

bool Parse(int argc, char *argv[], Args &args);

}; // namespace ArgParser
