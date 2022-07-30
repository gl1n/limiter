#include <arg_parser.h>
#include <container.h>

int main(int argc, char *argv[]) {
  ArgParser::Args args;
  if (ArgParser::Parse(argc, argv, args)) {
    Container::run(&args);
  }
  return 0;
}
