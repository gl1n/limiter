## Introduction
A linux container (toy) written in C/C++.

## How to use
1. extract busybox.tar to somewhere root user can access.
2. fetch limiter and compile it.
```
git clone https://
mkdir build && cd build
cmake ..
make
```
3. run limiter.
```
cd build 
sudo ./limiter run /path/to/busybox_dir sh
```

## Todo
- [x] friendly interface.
- [x] cpu constraint.
- [x] memory constraint.
- [x] running image.
- [x] user namespace mapping.
- [] image packing.
- [] bind mounting

## Note
This project was implemented based on cgroup v2. So be sure your linux is supporting cgroup v2 not v1.
You can run `./limiter help` to print help message.
