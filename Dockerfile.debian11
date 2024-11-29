FROM debian:11

ENV AWOO_MAKE_CONFIG="config.mk.dist"
ENV QEMU_FLAGS="-display none"
# ENV MAKEFLAGS="-j 2"
ENV CC=clang-13
ENV CLANG_CHECK=clang-check-13
ENV LD=ld.lld-13

#RUN apk add --no-cache binutils clang lld git llvm5 make nasm qemu qemu-system-i386 syslinux xorriso
#RUN apk add --no-cache wget unzip

RUN apt-get update && apt-get install -y clang-13 clang-tools-13 lld-13 git make nasm qemu qemu-system-x86 xorriso wget unzip
