#pragma once

#include <arg_parser.h>

#include <iostream>
#include <sched.h>
#include <signal.h>
#include <sys/mount.h>
#include <sys/wait.h>

namespace Container {

void run(ArgParser::Args *);
} // namespace Container
