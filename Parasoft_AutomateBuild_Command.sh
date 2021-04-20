#!/bin/sh

export PATH="/home/ruchika/TOOLCHAINS/gcc9.2/toolchains/aarch64/bin/":$PATH
export PATH="/home/ruchika/PARASOFT/cpptest/10.5":$PATH

cd /home/ruchika/MISRA/optee_os_ruchi393

make clean

rm -rf *.bdf

cpptesttrace --cpptesttraceOutputFile="/home/ruchika/MISRA/optee_os_ruchi393/optee_os_MISRA.bdf" --cpptesttraceProjectName="optee_os_MISRA" --cpptesttraceTraceCommand=aarch64-linux-gnu-gcc make O=out/arm CFG_ARM64_core=y PLATFORM=vexpress-qemu_armv8a CROSS_COMPILE=~/TOOLCHAINS/gcc9.2/toolchains/aarch64/bin/aarch64-linux-gnu- CROSS_COMPILE_core=~/TOOLCHAINS/gcc9.2/toolchains/aarch64/bin/aarch64-linux-gnu- CROSS_COMPILE_ta_arm64=~/TOOLCHAINS/gcc9.2/toolchains/aarch64/bin/aarch64-linux-gnu- CROSS_COMPILE_ta_arm32=~/TOOLCHAINS/gcc9.2/toolchains/aarch32/bin/arm-linux-gnueabihf- all

cpptest -clean
