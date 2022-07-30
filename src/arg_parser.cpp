#include <arg_parser.h>

#include <args.h>
#include <clipp.h>
#include <cstring>
#include <iostream>

bool ArgParser::Parse(int argc, char *argv[], Args &args) {
  enum class mode { run, help };
  mode selected = mode::run;
  std::vector<std::string> job;
  std::string hostname;

  auto runMode = (clipp::command("run").set(selected, mode::run),
                  clipp::values("job", job),
                  (clipp::option("--hostname").doc("set hostname") &
                   clipp::value("hostname", hostname)));

  auto helpMode = (clipp::command("help").set(selected, mode::help));

  auto cli = ((runMode | helpMode),
              clipp::option("-v", "--version")
                  .call([]() { std::cout << "version 0.1.0\n\n"; })
                  .doc("show version"));

  if (clipp::parse(argc, argv, cli)) {
    switch (selected) {
    case mode::run:
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
