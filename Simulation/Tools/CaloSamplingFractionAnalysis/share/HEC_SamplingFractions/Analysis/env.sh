#
export ATLAS_LOCAL_ROOT_BASE=/cvmfs/atlas.cern.ch/repo/ATLASLocalRootBase 
alias setupATLAS='source ${ATLAS_LOCAL_ROOT_BASE}/user/atlasLocalSetup.sh'
setupATLAS
#
lsetup "root 6.18.04-x86_64-centos7-gcc8-opt"
#
echo " " 
echo " === ROOTSYS: $ROOTSYS === "
echo " "
#
