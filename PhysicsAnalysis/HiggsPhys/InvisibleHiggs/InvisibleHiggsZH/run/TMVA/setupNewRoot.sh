# Set ROOTSYS to the newer ROOT directory
# For example (at CERN): /afs/cern.ch/sw/lcg/external/root/5.15.02/slc3_gcc323/root/ 
export ROOTSYS=/afs/cern.ch/sw/lcg/external/root/5.17.02/slc4_amd64_gcc34/root
export PATH=$ROOTSYS/bin/:$PATH
export LD_LIBRARY_PATH=${ROOTSYS}/lib:$LD_LIBRARY_PATH
export PYTHONPATH=${ROOTSYS}/lib:$PYTHONPATH
