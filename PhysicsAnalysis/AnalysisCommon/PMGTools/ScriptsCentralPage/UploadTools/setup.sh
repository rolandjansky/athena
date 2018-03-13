export ATLAS_LOCAL_ROOT_BASE=/cvmfs/atlas.cern.ch/repo/ATLASLocalRootBase
source /cvmfs/atlas.cern.ch/repo/ATLASLocalRootBase/user/atlasLocalSetup.sh --quiet
lsetup pyami rucio "asetup AthAnalysisBase,2.4.35,here"
voms-proxy-init -voms atlas
