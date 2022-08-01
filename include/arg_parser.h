#pragma once
#include <cgroup.h>

namespace ArgParser {

struct Args {
  char **command;
  char *hostname;
  int cpu_quota;
  int cpu_period;
  int memory;
  int swap;

  char *image;

  int pipe_fd[2];

  Args()
      : command(nullptr), hostname(nullptr), cpu_quota(0), cpu_period(0),
        image(nullptr) {}

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
