#!/bin/bash
set -e

# Working dir within the docker container (should not change at all):
WORKING_DIR="/root/bose-soundtouch-hook"

cd $HOST_PROJECT_DIR/..
# Copy files to docker container:
CONTAINER_ID=$(docker ps --filter "ancestor=$DOCKER_IMAGE_NAME" --format "{{.ID}}" | head -n1)
if [ -z "$CONTAINER_ID" ]; then
  echo "error: no docker container for image $DOCKER_IMAGE_NAME found, please start it first manually via:" >&2
  echo "       First, scp file assets/docker-bose-build-env.tar to your docker host, then:" >&2
  echo "       $ docker load -i docker-bose-build-env.tar" >&2
  echo "       $ docker run -d -it --platform linux/arm/v7 --name bose-builder $DOCKER_IMAGE_NAME bash" >&2
  exit 1
fi
echo "Copying files to docker container $CONTAINER_ID of image $DOCKER_IMAGE_NAME ..."
docker exec $CONTAINER_ID rm -rf $WORKING_DIR
docker cp bose-soundtouch-hook $CONTAINER_ID:$WORKING_DIR
# (Cross-)Compile inside docker container and transfer built files to Bose speaker:
echo "Switching to docker container $CONTAINER_ID ..."
docker exec "$CONTAINER_ID" \
  env WORKING_DIR=$WORKING_DIR TARGET_DEVICE=$TARGET_DEVICE OUTPUT_FILE=$OUTPUT_FILE \
  bash -c 'cd "$WORKING_DIR" && ./scripts/utils/build-and-transfer.sh'