# Getting a build env for compiling for the Bose box:

Note that there is a prebuild fully working docker image in `assets/` to simplify/skip the following process.

```shell
# Start a Debian Wheezy ARMv7 container using QEMU (you end up in a bash shell); do this on a x86_64 host:
sudo apt install docker.io
sudo apt install qemu-user-static
sudo docker run -it --platform linux/arm/v7 debian:wheezy bash

# System setup in Docker (Debian Wheezy ARMv7):
# (the following happens in the docker container's shell)
# Remove existing sources.list
echo "" > /etc/apt/sources.list

# Add archive sources
echo "deb http://archive.debian.org/debian/ wheezy main" >> /etc/apt/sources.list
echo "deb-src http://archive.debian.org/debian/ wheezy main" >> /etc/apt/sources.list

# Disable validity check of the time (since Wheezy is expired)
echo 'Acquire::Check-Valid-Until "false";' > /etc/apt/apt.conf.d/99no-check-valid-until

# Install compiler
apt-get update
apt-get install -y --force-yes gcc 
# For scp, file and nano:
apt-get -y --force-yes install openssh-client file nano

# Install missing packages (see also assets/ dir):
wget http://archive.debian.org/debian/pool/main/e/eglibc/libc6-dev_2.13-38+deb7u10_armhf.deb
wget wget http://ftp.sjtu.edu.cn/sites/archive.debian.org/debian/pool/main/l/linux/linux-libc-dev_3.2.78-1_armhf.de
wget http://archive.debian.org/debian/pool/main/e/eglibc/libc-dev-bin_2.13-38+deb7u10_armhf.deb

# Compile and copy the shared library to the target device (copy/create ssl_spy.c first):
gcc -shared -fPIC -o libbose_hook.so ssl_spy.c -ldl
scp libbose_hook.so root@{YOUR_SOUNDTOUCH_DEVICE}:/tmp/libbose_hook.so
```

# Enabling permanent ssh+telnet access:
~Simply change `true` to `false` in `/usr/bin/is-production`. Before doing so, use `rw` to enable write-access. Then `reboot`.~

Simply do `touch /mnt/nv/remote_services` once.
