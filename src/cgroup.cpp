#include <cgroup.h>
#include <errno.h>
#include <fstream>
#include <iostream>
#include <sole.hpp>
#include <sys/stat.h>

Cgroup::Cgroup(int pid) : mCgroupDir("/sys/fs/cgroup/" + sole::uuid1().str()) {
  if (mkdir(mCgroupDir.data(), 0755) == -1) {
    perror("mkdir error: ");
  }
  std::ofstream proc(mCgroupDir + "/cgroup.procs");
  proc << pid;
  proc.close();
}

Cgroup::~Cgroup() {
  if (rmdir(mCgroupDir.data()) == -1) {
    perror("rmdir error: ");
  }
}

void Cgroup::LimitCPU(int cpu_period, int cpu_quota) {
  if (cpu_quota && cpu_period) {
    std::ofstream cg(mCgroupDir + "/cpu.max", std::ios::trunc);
    cg << cpu_quota << " " << cpu_period;
    cg.close();
  }
}
void Cgroup::LimitMem(int memory, int swap) {
  if (memory == -1) {
    return;
  }
  std::ofstream mem_cg(mCgroupDir + "/memory.max", std::ios::trunc);
  mem_cg << memory;
  mem_cg.close();

  std::ofstream swap_cg(mCgroupDir + "/memory.swap.max", std::ios::trunc);
  if (swap != -1) {
    swap_cg << swap;
  } else {
    swap_cg << memory;
  }
  swap_cg.close();
}
