FROM centos:7 as glibc
RUN yum -y update && yum -y install make git gcc-c++ java-11-openjdk-devel glibc-static
ADD ./src /asprof/src
ADD ./Makefile ./LICENSE ./*.md ./JavaHome.class /asprof/
WORKDIR /asprof
RUN make release


FROM alpine:3.13 as musl
RUN apk add gcc g++ make openjdk11 linux-headers
ADD ./src /asprof/src
ADD ./Makefile ./LICENSE ./*.md ./JavaHome.class /asprof/
WORKDIR /asprof
RUN make release


FROM scratch
COPY --from=glibc /asprof/*.tar.gz /
COPY --from=musl /asprof/*.tar.gz /