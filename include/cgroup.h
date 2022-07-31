#pragma once

#include <string>

class Cgroup {
public:
  Cgroup(int pid);
  ~Cgroup();

  void LimitCPU(int cpu_quota, int cpu_period);
  void LimitMem(int memory, int swap);

private:
  std::string mCgroupDir;
};
