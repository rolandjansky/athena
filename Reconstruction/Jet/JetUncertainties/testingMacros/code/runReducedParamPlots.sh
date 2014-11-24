#!/bin/bash

JESprovPath="../.."

if [[ ! $LD_LIBRARY_PATH = *$JESprovPath/StandAlone* ]] ; then
  export LD_LIBRARY_PATH=$JESprovPath/StandAlone:$LD_LIBRARY_PATH
fi

flags=$($ROOTSYS/bin/root-config --cflags --glibs)
includes="-I$JESprovPath -L$JESprovPath/StandAlone -lJetUncertainties"


plotExec="ProduceReductionPlots.exe"
plotSource="ProduceReductionPlots.C"

#algorithms="AntiKt4TopoEM,AntiKt4LCTopo,AntiKt6TopoEM,AntiKt6LCTopo"
algorithms="AntiKt4TopoEM"
#MCtype="MC12a"
MCtype="MC11b"

JESsharePath="../../share"
#filePath="JES_2012/Moriond2013"
filePath="JES_2011/Final"
#filePrefix="InsituJES2012"
filePrefix="InsituJES2011"

if test $# -ne 1 ; then
  echo -e "USAGE: $0 configFile"
  exit 1
fi

g++ -o $plotExec $plotSource $flags $includes && {
  echo "Successfully compiled comparison code"
} || {
  echo "Failed to compile comparison code.  Exiting..."
  exit -3
}



./$plotExec $filePath/${filePrefix}_$1 $algorithms $MCtype $JESsharePath 0.5 Public
if test $? -ne 0 ; then
  echo -e "\nUnsuccessful execution of reduction plotting program.  Exiting...\n"
  exit 3
fi
echo -e "\nFinished executing reduction plotting program!\n"
