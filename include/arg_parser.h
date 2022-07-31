#pragma once

namespace ArgParser {

struct Args {
  char **job;
  char *hostname;
  int cpu_quota;
  int cpu_period;

  Args() : job(nullptr), hostname(nullptr), cpu_quota(0), cpu_period(0) {}

  ~Args() {
    if (job) {
      for (char **p = job; *p; p++) {
        delete[] * p;
      }
      delete[] job;
    }

    delete[] hostname;
  };
};

bool Parse(int argc, char *argv[], Args &args);

}; // namespace ArgParser
