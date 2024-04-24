#!/bin/bash
APIDIR="./api"
protoc \
	--proto_path=${APIDIR} \
	--grpc_out=${APIDIR} --plugin=protoc-gen-grpc=`which grpc_cpp_plugin` \
	--cpp_out=${APIDIR} \
	${APIDIR}/*.proto