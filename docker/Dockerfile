FROM goodbaikin/grpc-builder:v0.10
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

FROM goodbaikin/amatsukaze:v0.14
COPY --from=0 /yukikaze/YukikazeServer/build/yukikaze_server /usr/local/bin
COPY --from=0 /yukikaze/YukikazeClient/build/yukikaze_client /usr/local/bin