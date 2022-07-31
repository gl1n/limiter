#include <arg_parser.h>

#include <clipp.h>
#include <cstring>
#include <iostream>
#include <unistd.h>

bool ArgParser::Parse(int argc, char *argv[], Args &args) {
  enum class mode { run, help };
  mode selected = mode::run;
  std::vector<std::string> job;
  std::string hostname;
  int cpu_quota;
  int cpu_period;
  int memory = -1;
  int swap = -1;

  auto runMode =
      (clipp::command("run").set(selected, mode::run),
       clipp::values("job", job),
       (clipp::option("--hostname").doc("set hostname") &
        clipp::value("hostname", hostname)),
       (clipp::option("--cpu-quota").doc("set cpu quota") &
        clipp::value("quota", cpu_quota)),
       (clipp::option("--cpu-period").doc("set cpu period") &
        clipp::value("period", cpu_period)),
       (clipp::option("--memory", "-m").doc("set memory limit (unit=MB)") &
        clipp::value("memory size", memory)),
       (clipp::option("--swap-memory").doc("set swap memory limit (unit=MB))") &
        clipp::value("swap size", swap)));

  auto helpMode = (clipp::command("help").set(selected, mode::help));

  auto cli = ((runMode | helpMode),
              clipp::option("-v", "--version")
                  .call([]() { std::cout << "version 0.1.0\n\n"; })
                  .doc("show version"));

  if (clipp::parse(argc, argv, cli)) {
    switch (selected) {
    case mode::run:
      //没有root权限直接exit
      if (geteuid() != 0) {
        std::cerr << "Got permisson denied when running " << argv[0]
                  << ". Please run " << argv[0] << " with root privilege."
                  << std::endl;
        exit(EXIT_FAILURE);
      }
      // get hostname
      if (hostname.size()) {
        args.hostname = new char[hostname.size() + 1];
        strcpy(args.hostname, hostname.data());
      }
      // get job
      args.job = new char *[job.size() + 1];
      for (size_t i = 0; i != job.size(); i++) {
        args.job[i] = new char[job[i].size() + 1];
        strcpy(args.job[i], job[i].data());
      }
      args.job[job.size()] = nullptr;
      // get cpu limit
      args.cpu_period = cpu_period;
      args.cpu_quota = cpu_quota;
      // get memory limit
      args.memory = memory;
      args.swap = swap;

      break;

    case mode::help:
      std::cout << clipp::make_man_page(cli, argv[0]);
      break;
    }
    return true;
  }
  std::cout << clipp::usage_lines(cli, argv[0]) << "\n";
  return false;
}
