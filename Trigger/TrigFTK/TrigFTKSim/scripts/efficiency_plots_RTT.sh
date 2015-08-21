#!/bin/sh

TEMPDIR=$PWD
echo "working dir is $TEMPDIR"
cd $AtlasBaseDir/*/$AtlasVersion/Trigger/TrigFTK/TrigFTKSim/standalone
echo "compiling code in $PWD"
setupATLAS
source bootstrap.sh
make efficiency
cd $TEMPDIR
$AtlasBaseDir/*/$AtlasVersion/Trigger/TrigFTK/TrigFTKSim/standalone/efficiency --files tempout.root


