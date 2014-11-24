#!/bin/sh
exec=bin/test-UJU.exe
code=code/test-UJU.cpp
mkdir -p bin/

flags=$($ROOTSYS/bin/root-config --cflags --glibs)
includes="-I../ -L../StandAlone -I. -lJetUncertainties"

export LD_LIBRARY_PATH=../StandAlone:$LD_LIBRARY_PATH
  # needed for mac
export DYLD_LIBRARY_PATH=../StandAlone:$DYLD_LIBRARY_PATH

cd ../cmt
make -f Makefile.Standalone
cd -

echo ; echo "Will compile $code"
g++ -o $exec $code $flags $includes && {
    echo "Compilation successful!"
    echo
    echo "Will now run..."
    #echo $exec ../share/UJ_2012/UJInput2012.root
    #./$exec ../share/UJ_2012/UJInput2012.root
    echo $exec ../share/UJ_2012/JMS2012_Sep232013_DoubleRatio.root
    $exec ../share/UJ_2012/JMS2012_Sep232013_DoubleRatio.root
} || {
    echo "Compilation failed :("
    echo
}
