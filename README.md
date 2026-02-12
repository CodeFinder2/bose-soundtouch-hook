# Overview
This project is used to analyze the (encrypted) traffic between Bose SoundTouch speaker devices and the Bose cloud servers which are expected to be shut down in May 2026.

With the help of this tooling, I was already able to intercept and read the traffic transmitted and received when playing notifications. With these information, a Python test server was created and the [`STSCertified` process](./docs/STSCertified-process-analyses.md) was hooked using `LD_PRELOAD` in order to direct all traffic to the test server, effectively bypassing Bose's cloud servers. That way, this functionality will still work after cloud shutdown. Nonetheless, this is not meant to be as a convenient integration (yet?) but rather for reverse engineering purposes only. There's still a lot of analysis required.

Conceptually, the docker container image (provided in `assets/`) in this repo provides a cross-compilation build environment. It therefore also allows writing new software for the devices!

Eventually, all this should be integrated into the [SoundCork project](https://github.com/deborahgu/soundcork).

Preliminary analysis results can be found in `docs/`.

# Build environment for cross-compilation
You can get a cross compilation environment with:
```shell
cd $YOUR_GIT_ROOT/scripts/utils
./reassemble-docker-image.sh
# > Archive docker-bose-build-env-image.tar OK
# (ensure that docker daemon is running in the background, e.g., via open -a Docker)
docker load -i docker-bose-build-env-image.tar
# > Loaded image: bose-build-env:wheezy
docker run -d -it --platform linux/arm/v7 --name bose-builder bose-build-env:wheezy bash
# > e3e4e3789b4d8724ad4bde4ade3a0991b045f0e8f361aa18aa66dc683494410b
docker exec -it bose-builder /bin/bash
# > root@e3e4e3789b4d:/# cd /root/ && ls
# > build.sh  libssl_spy.so  ssl_spy.c
```
Using that container, the `scripts/deploy.sh` script allows for convenient cross-compiling and deployment of the code in this repo to a real device.
