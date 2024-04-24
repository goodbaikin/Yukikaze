FROM ubuntu:22.04
RUN apt-get update &&\
	apt-get install -y \
		git \
		cmake \
		build-essential \
		pkg-config \
		libtool \
		autoconf \
		libssl-dev

RUN cd / && git clone -b v1.62.0 --depth 1 https://github.com/grpc/grpc && \
	cd /grpc && \
	git submodule update --init && \
	mkdir -p cmake/build && \
	cd cmake/build && \
	cmake -DgRPC_INSTALL=ON \
			-DgRPC_BUILD_TESTS=OFF \
			-DCMAKE_POSITION_INDEPENDENT_CODE=TRUE \
			../.. && \
	make -j$(nproc)&& \
	make install && \
	ldconfig

RUN cp -r /grpc/third_party/abseil-cpp/absl /usr/local/include/
RUN cp -r /grpc /usr/local/include/

ARG VERSION=master
ENV VERSION=${VERSION}

RUN git clone --depth 1 -b ${VERSION} https://github.com/goodbaikin/yukikaze
RUN cd yukikaze &&\
	./codegen.sh

RUN cd yukikaze/YukikazeServer &&\
	mkdir build &&\
	cd build &&\
	cmake .. &&\
	make -j$(nproc)

RUN cd yukikaze/YukikazeClient &&\
	mkdir build &&\
	cd build &&\
	cmake .. &&\
	make -j $(nproc)

FROM goodbaikin/amatsukaze:v0.13
COPY --from=0 /yukikaze/YukikazeServer/build/yukikaze_server /usr/local/bin
COPY --from=0 /yukikaze/YukikazeClient/build/yukikaze_client /usr/local/bin