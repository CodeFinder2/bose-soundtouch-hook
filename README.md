# Overview
This project is used to analyze the (encrypted) traffic between Bose SoundTouch speaker devices and the Bose cloud servers which are expected to be shut down in May 2026.

With the help of this tooling, I was already able to intercept and read the traffic transmitted and received when playing notifications. With these information, a Python test server was created and the [`STSCertified` process](./docs/STSCertified-process-analyses.md) was hooked using `LD_PRELOAD` in order to direct all traffic to the test server, effectively bypassing Bose's cloud servers. That way, this functionality will still work after cloud shutdown. Nonetheless, this is not meant to be as a convenient integration (yet?) but rather for reverse engineering purposes only. There's still a lot of analysis required.

Conceptually, the docker container image (provided in `assets/`) in this repo provides a cross-compilation build environment. It therefore also allows writing new software for the devices!

Eventually, all this should be integrated into the [SoundCork project](https://github.com/deborahgu/soundcork).

Preliminary analysis results can be found in `docs/`.
