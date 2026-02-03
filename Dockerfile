# Image for building async-profiler release packages (Grafana fork)
# Uses upstream's pre-built builder image which has Debian 10 + GCC 8.3 + musl toolchain
# This ensures compatibility with both glibc and musl-based systems (Alpine)

FROM public.ecr.aws/async-profiler/asprof-builder-x86:latest AS builder-amd64
FROM public.ecr.aws/async-profiler/asprof-builder-arm:latest AS builder-arm64

ARG TARGETARCH
FROM builder-${TARGETARCH} AS builder

RUN mkdir -p /asprof
ADD ./src /asprof/src
ADD ./Makefile ./LICENSE ./*.md ./JavaHome.class /asprof/
WORKDIR /asprof
RUN make CC=/usr/local/musl/bin/musl-gcc release

FROM scratch
COPY --from=builder /asprof/*.tar.gz /
