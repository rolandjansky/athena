export ATLAS_LOCAL_ROOT_BASE=/cvmfs/atlas.cern.ch/repo/ATLASLocalRootBase
source ${ATLAS_LOCAL_ROOT_BASE}/user/atlasLocalSetup.sh

setupATLAS
asetup AtlasDerivation 20.7.6.4,here

alias cdl='cd $TestArea/PhysicsAnalysis/MuonID/MuonPerformanceAnalysis/MuonPerformanceAlgs/'
