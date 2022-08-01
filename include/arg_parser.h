#pragma once
#include <cgroup.h>

namespace ArgParser {

struct Args {
  char **job;
  char *hostname;
  int cpu_quota;
  int cpu_period;
  int memory;
  int swap;

  char *image;

  Args()
      : job(nullptr), hostname(nullptr), cpu_quota(0), cpu_period(0),
        image(nullptr) {}

  ~Args() {
    if (job) {
      for (char **p = job; *p; p++) {
        delete[] * p;
      }
      delete[] job;
    }

    delete[] hostname;
    delete[] image;
  };
};

bool Parse(int argc, char *argv[], Args &args);

}; // namespace ArgParser
