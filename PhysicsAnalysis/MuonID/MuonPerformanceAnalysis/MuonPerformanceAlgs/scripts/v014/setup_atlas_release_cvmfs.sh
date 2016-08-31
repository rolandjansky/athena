export ATLAS_LOCAL_ROOT_BASE=/cvmfs/atlas.cern.ch/repo/ATLASLocalRootBase
source ${ATLAS_LOCAL_ROOT_BASE}/user/atlasLocalSetup.sh

setupATLAS
asetup AtlasProduction 20.1.7.2,here

alias cdl='cd $TestArea/PhysicsAnalysis/MuonID/MuonPerformanceAnalysis/MuonPerformanceAlgs/'