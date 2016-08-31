export ATLAS_LOCAL_ROOT_BASE=/cvmfs/atlas.cern.ch/repo/ATLASLocalRootBase
source ${ATLAS_LOCAL_ROOT_BASE}/user/atlasLocalSetup.sh

setupATLAS
asetup AtlasProduction 20.7.3.8,here

alias cdl='cd $TestArea/PhysicsAnalysis/MuonID/MuonPerformanceAnalysis/MuonPerformanceAlgs/'