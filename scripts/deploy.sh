#!/usr/bin/env bash
set -euo pipefail

# Define target filename:
OUTPUT_FILE="libbose_hook.so"
# Define target Bose speaker (IP address or hostname):
TARGET_DEVICE="soundtouch-bad.lan"
# Define docker host (can be localhost as well; suggested to use public-key authentication):
DOCKER_HOST="abo@ubuntu-docker"
# Define project directory on the docker host
HOST_PROJECT_DIR="/home/abo/bose-soundtouch-hook"
# Define docker image name (must be available on the docker host):
DOCKER_IMAGE_NAME="bose-build-env:wheezy"

while [[ $# -gt 0 ]]; do
  case "$1" in
    --output)
      OUTPUT_FILE="$2"
      shift 2
      ;;
    --target)
      TARGET_DEVICE="$2"
      shift 2
      ;;
    --host)
      DOCKER_HOST="$2"
      shift 2
      ;;
    *)
      echo "error: unknown argument $1" >&2
      exit 1
      ;;
  esac
done

# Transfer files to server hosting docker containers:
echo "Transferring files to docker host $DOCKER_HOST ..."
ssh $DOCKER_HOST "rm -rf $HOST_PROJECT_DIR"
# Copy entire Git repo to docker host (temporary)
scp -rq ../../bose-soundtouch-hook $DOCKER_HOST:$HOST_PROJECT_DIR
# Execute remote cross-compile script:
ssh $DOCKER_HOST \
  "env \
    TARGET_DEVICE=$TARGET_DEVICE \
    OUTPUT_FILE=$OUTPUT_FILE \
    HOST_PROJECT_DIR=$HOST_PROJECT_DIR \
    DOCKER_IMAGE_NAME=$DOCKER_IMAGE_NAME \
    bash -s" \
  < ./utils/cross-compile-remote.sh
echo "Done."
