#!/bin/sh
exec=bin/jes_uju_plots.exe
code=code/MakeUJUncertaintyPlots.C
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
    for i in JMS2012_Apr042014_Combined.root;
       do
       echo "For "${i}
       echo ""
       #for j in AntiKt10LCTopo AntiKt10LCTopoTrimmedPtFrac5SmallR30 CamKt12LCTopo CamKt12LCTopoSplitFilteredmassFraction67minSplitR0;
       for j in AntiKt10LCTopoTrimmedPtFrac5SmallR30;
       do
          ./$exec '../share/UJ_2012/'${i} ${j}
       done
    done
} || {
    echo "Compilation failed :("
    echo
}
