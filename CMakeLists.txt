cmake_minimum_required(VERSION 3.22)
project(async-profiler)

set(CMAKE_CXX_STANDARD 11)
# -fno-omit-frame-pointer -fvisibility=hidden -Wl,-z,defs -fwhole-program -momit-leaf-frame-pointer
add_library(async-profiler
        ./src/arguments.cpp
        ./src/flameGraph.cpp
        ./src/profiler.cpp
        ./src/trap.cpp
        ./src/stackWalker.cpp
        ./src/symbols_macos.cpp
        ./src/tsc.cpp
        ./src/j9Ext.cpp
        ./src/codeCache.cpp
        ./src/itimer.cpp
        ./src/wallClock.cpp
        ./src/stackFrame_x64.cpp
        ./src/j9StackTraces.cpp
        ./src/mutex.cpp
        ./src/flightRecorder.cpp
        ./src/javaApi.cpp
        ./src/j9WallClock.cpp
        ./src/stackFrame_arm.cpp
        ./src/instrument.cpp
        ./src/j9ObjectSampler.cpp
        ./src/os_linux.cpp
        ./src/lockTracer.cpp
        ./src/dictionary.cpp
        ./src/fdtransferClient_linux.cpp
        ./src/perfEvents_linux.cpp
        ./src/linearAllocator.cpp
        ./src/stackFrame_i386.cpp
        ./src/stackFrame_ppc64.cpp
        ./src/jfrMetadata.cpp
        ./src/threadFilter.cpp
        ./src/dwarf.cpp
        ./src/log.cpp
        ./src/objectSampler.cpp
        ./src/os_macos.cpp
        ./src/stackFrame_aarch64.cpp
        ./src/engine.cpp
        ./src/vmStructs.cpp
        ./src/symbols_linux.cpp
        ./src/frameName.cpp
        ./src/vmEntry.cpp
        ./src/callTraceStorage.cpp
        ./src/allocTracer.cpp
        )
target_compile_options(async-profiler PRIVATE -fno-omit-frame-pointer -fvisibility=hidden -Wl,-z,defs -fwhole-program -momit-leaf-frame-pointer -fPIC)
target_compile_definitions(async-profiler PRIVATE PROFILER_VERSION=\"0.0.0\")
target_include_directories(async-profiler PRIVATE
        ./src
        ./src/res
        ./src/helper
        /usr/lib/jvm/java-11-openjdk-amd64/include
        /usr/lib/jvm/java-11-openjdk-amd64/include/linux)

# -DPROFILER_VERSION=\"2.9.0.2\" -I/usr/lib/jvm/java-11-openjdk-amd64/include -Isrc/res -Isrc/helper -I/usr/lib/jvm/java-11-openjdk-amd64/include/linux -fPIC -shared -o
