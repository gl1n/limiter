#pragma once

#include <args.h>

#include <iostream>
#include <sched.h>
#include <signal.h>
#include <sys/mount.h>
#include <sys/wait.h>

namespace Container {

void run(Args *);
} // namespace Container
