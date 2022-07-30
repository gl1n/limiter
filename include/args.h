#pragma once

struct Args {
  char **job;
  char *hostname;

  Args() : job(nullptr), hostname(nullptr) {}

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
