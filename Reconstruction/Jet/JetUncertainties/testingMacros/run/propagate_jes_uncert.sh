#!/bin/sh
exec=bin/run_propagate_jes.exe
code=code/PropagateJES.C

JESconf=${1:-JES_2012/JESUncertainty2012_Sept2012.config}
MJEScomf=${2:-JES_2012/MultijetJES_2012.config}

mkdir -p bin

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
    echo "Compliation successful!\n";
    echo "Will now run..."
    ./$exec $JESconf $MJESconf
    echo "Done" ;
} || {
    echo "Compilation failed :(" ; echo ;
}
