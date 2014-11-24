#!/bin/bash

JESprovPath="../.."

if [[ ! $LD_LIBRARY_PATH = *$JESprovPath/StandAlone* ]] ; then
  export LD_LIBRARY_PATH=$JESprovPath/StandAlone:$LD_LIBRARY_PATH
fi

flags=$($ROOTSYS/bin/root-config --cflags --glibs)
includes="-I$JESprovPath -L$JESprovPath/StandAlone -lJetUncertainties"


compareExec="jes_corr_compare.exe"
compareSource="CompareCorrelations.C"

#algorithms="AntiKt4TopoEM,AntiKt4LCTopo,AntiKt6TopoEM,AntiKt6LCTopo"
algorithms="AntiKt4TopoEM,AntiKt4LCTopo"
MCtype="MC12a"
#MCtype="MC11b"

JESsharePath="../../share"
filePath="JES_2012/Final"
#filePath="JES_2011/Final"
filePrefix="InsituJES2012"
#filePrefix="InsituJES2011"

if test $# -ne 2 ; then
  echo -e "USAGE: $0 configFile1 configFile2"
  exit 1
fi

g++ -o $compareExec $compareSource $flags $includes && {
  echo "Successfully compiled comparison code"
} || {
  echo "Failed to compile comparison code.  Exiting..."
  exit -3
}



./$compareExec $JESsharePath $filePath/${filePrefix}_$1 $filePath/${filePrefix}_$2 $algorithms $MCtype
if test $? -ne 0 ; then
  echo -e "\nUnsuccessful execution of comparison program.  Exiting...\n"
  exit 3
fi
echo -e "\nFinished executing correlation matrix comparison program!\n"
