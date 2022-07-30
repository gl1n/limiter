#include <arg_parser.h>
#include <args.h>
#include <container.h>

int main(int argc, char *argv[]) {
  Args args;
  if (ArgParser::Parse(argc, argv, args)) {
    Container::run(&args);
  }
  return 0;
}
