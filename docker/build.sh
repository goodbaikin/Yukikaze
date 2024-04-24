#!/bin/bash
VER=`git describe --abbrev=0 --tags`
IMAGE=goodbaikin/yukikaze
sudo $(which docker) build -t ${IMAGE}:${VER} --build-arg VERSION=${VER} .