export ATLAS_LOCAL_ROOT_BASE=/cvmfs/atlas.cern.ch/repo/ATLASLocalRootBase
source ${ATLAS_LOCAL_ROOT_BASE}/user/atlasLocalSetup.sh

setupATLAS
asetup 20.7.X.Y-VAL-AtlasDerivation, AtlasDerivation, latest,here

alias cdl='cd $TestArea/PhysicsAnalysis/MuonID/MuonPerformanceAnalysis/MuonPerformanceAlgs/'
