#!/bin/bash


echo  "[systematics-tool setup] --- go to working directory ---"
cd !PWD!
echo  "[systematics-tool setup] --- [DONE] ---"

echo  "[systematics-tool setup] --- setup Athena release  21, including Rivet ---"
# sources the latest rivet version compatible with athena
setupATLAS
asetup 21.6.33,AthGeneration # loads Rivet
source ${LCG_RELEASE_BASE}/LCG_88/MCGenerators/rivet/${RIVETVER}/${LCG_PLATFORM}/rivetenv.sh
export RIVET_ANALYSIS_PATH=$RIVET_ANALYSIS_PATH:${LCG_RELEASE_BASE}/LCG_88/MCGenerators/rivet/${RIVETVER}/${LCG_PLATFORM}/share/Rivet
source data/setupLHAPDF.sh
echo  "[systematics-tool setup] --- [DONE] ---"

echo  "[systematics-tool setup] --- setup panda and rucio for GRID submissions ---"
lsetup panda
lsetup rucio
voms-proxy-init -voms atlas
echo  "[systematics-tool setup] --- [DONE] ---"


echo  "[systematics-tool setup] --- set env variable ---"
export SYSTTOOLSPATH="!PWD!" 
echo  "[systematics-tool setup] --- [DONE] ---"

echo  "[systematics-tool setup] --- append to PATH environment variable  ---"
# this is so that:
# a) latex compiles correctly and
# b) you can call the systematics-tools commands from anywhere 
export PATH=/cvmfs/sft.cern.ch/lcg/external/texlive/2016/bin/x86_64-linux:$PATH
export PATH="$PWD/local/bin:$PATH"
echo  "[systematics-tool setup] --- [DONE] ---"

echo  "[systematics-tool setup] --- append to PYTHONPATH environment variable  ---"
# this is so that one can import the systematics-tools as function
# librarirs in user python scripts
#export PYTHONPATH=$PWD/local/lib64/python2.7/site-packages:$PYTHONPATH
#export PYTHONPATH=$PWD/local/lib64/python2.6/site-packages:$PYTHONPATH
export PYTHONPATH="$PWD/local/bin:$PYTHONPATH" 
export PYTHONPATH="/cvmfs/atlas.cern.ch/repo/sw/software/21.6/sw/lcg/releases/scipy/0.18.1-f4cba/x86_64-slc6-gcc62-opt/lib/python2.7/site-packages/:$PYTHONPATH" 
export PYTHONPATH="/cvmfs/atlas.cern.ch/repo/sw/software/21.6/sw/lcg/releases/matplotlib/1.5.1-763af/x86_64-slc6-gcc62-opt/lib/python2.7/site-packages/:$PYTHONPATH" 
echo  "[systematics-tool setup] --- [DONE] ---"

# workaround for numpy on cc7 
# https://twiki.cern.ch/twiki/bin/view/AtlasComputing/CentOS7Readiness#ATLAS_software_status

if [ -e /usr/lib64/atlas/libsatlas.so ]; then
   workaroundLib="`pwd`/extraLibs"
   if [ ! -e $workaroundLib ]; then
     mkdir -p $workaroundLib
     ln -s /usr/lib64/atlas/libsatlas.so $workaroundLib/libptf77blas.so.3
     ln -s /usr/lib64/atlas/libsatlas.so $workaroundLib/libptcblas.so.3
     ln -s /usr/lib64/atlas/libsatlas.so $workaroundLib/libatlas.so.3
     ln -s /usr/lib64/atlas/libsatlas.so $workaroundLib/liblapack.so.3 
   # do the same for any other atlas lib that is missing and needed
   fi
   export LD_LIBRARY_PATH="$LD_LIBRARY_PATH:$workaroundLib"
fi

echo  "[systematics-tool setup] --- [DONE] ---"

