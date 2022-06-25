FROM centos:7

RUN yum -y update && yum -y install make git gcc-c++ java-11-openjdk-devel

