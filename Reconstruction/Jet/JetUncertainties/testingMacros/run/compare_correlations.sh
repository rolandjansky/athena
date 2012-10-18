#!/bin/sh
exec=bin/jes_corr_compare.exe
code=code/CompareCorrelations.C
mkdir -p bin

ref=${1:-JES_2012/JESUncertainty2012_FirstData.config}
probe=${2:-JES_2012/JESUncertainty2012_Sept2012.config}

flags=$($ROOTSYS/bin/root-config --cflags --glibs)
includes="-I../ -L../StandAlone -lJetUncertainties"

echo ; echo "Will compile the JESUncertaintyProvider"
cd ../cmt
make -f Makefile.Standalone
cd -

export LD_LIBRARY_PATH=../StandAlone:$LD_LIBRARY_PATH
export DYLD_LIBRARY_PATH=../StandAlone:$LD_LIBRARY_PATH:$DYLD_LIBRARY_PATH

echo ; echo "Will compile $code"

rm -f $exec
g++ -o $exec $code $flags $includes && {
    echo "Compliation successful!"; echo ;
    ./$exec $ref $probe
    echo ;
} || {
    echo "Compilation failed :("
    echo
}
