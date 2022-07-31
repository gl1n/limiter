#include <cgroup.h>
#include <errno.h>
#include <fstream>
#include <iostream>
#include <sole.hpp>
#include <sys/stat.h>

Cgroup::Cgroup(int pid) : mCgroupDir("/sys/fs/cgroup/" + sole::uuid1().str()) {
  if (mkdir(mCgroupDir.data(), 0755) == -1) {
    std::cerr << "mkdir error: " << strerror(errno) << std::endl;
  }
  std::ofstream proc(mCgroupDir + "/cgroup.procs");
  proc << pid;
  proc.close();
}

Cgroup::~Cgroup() {
  if (rmdir(mCgroupDir.data()) == -1) {
    std::cerr << "rmdir error: " << strerror(errno) << std::endl;
  }
}

void Cgroup::LimitCPU(int cpu_period, int cpu_quota) {
  if (cpu_quota && cpu_period) {
    std::ofstream cg(mCgroupDir + "/cpu.max", std::ios::trunc);
    cg << cpu_quota << " " << cpu_period;
    cg.close();
  }
}
void Cgroup::LimitMem(std::string memory, std::string swap) {}
