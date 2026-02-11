TARGET=root@soundtouch-bad.lan

scp -O -o HostKeyAlgorithms=+ssh-rsa -o PubkeyAcceptedAlgorithms=+ssh-rsa $1 $TARGET:/tmp/
