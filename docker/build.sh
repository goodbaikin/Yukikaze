#!/bin/bash
VER=`git describe --abbrev=0 --tags`
sudo $(which docker) buildx build --load -t goodbaikin/yukikaze:${VER} \
	--cache-to type=registry,ref=goodbaikin/yukikaze:cache \
	--cache-from type=registry,ref=goodbaikin/yukikaze:cache \
	--build-arg VERSION=${VER} .