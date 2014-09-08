#! /bin/csh

setenv ATLAS_ROOT /afs/cern.ch/atlas/software
setenv CVSROOT $ATLAS_ROOT/cvs
setenv PATH "$PATH":"$ATLAS_ROOT"/bin
source `srt setup -s csh` 

setenv SRT_RELEASE 1.3.2
srt new src $SRT_RELEASE

cd src
cvs checkout -d LArROD offline/LArCalorimeter/LArROD
#cvs checkout -d LArBookkeeping offline/LArCalorimeter/LArBookkeepingcd ..
cd ..
mkdir build
cd build
../src/configure
#make clean 
make install
cd ..
mkdir run
cd run
source ../src/LArROD/misc/runme.csh
athena
