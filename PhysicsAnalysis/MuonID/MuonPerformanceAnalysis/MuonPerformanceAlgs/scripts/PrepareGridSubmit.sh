# before submitting to the grid:

# A) setup your release (replace release number) and Panda/Emi at once:
#lsetup "asetup 20.7.7.4, AtlasDerivation, here" Panda Emi
lsetup Panda Emi

# B) create voms proxy:
voms-proxy-init -voms atlas:/atlas/perf-muons/Role=production

# C) Prepare grl:
#cp /afs/cern.ch/user/a/atlasdqm/grlgen/All_Good/data15_13TeV.periodAllYear_DetStatus-v63-pro18-01_DQDefects-00-01-02_PHYS_StandardGRL_All_Good.xml grl.xml

# D) Copy the scale-factors to be used for the closure to the testarea folder
#cp -r /cvmfs/atlas.cern.ch/repo/sw/database/GroupData/MuonEfficiencyCorrections/Data15AC_150806 ScaleFactors

# now proceed with 'source makeTarBalls.sh <input file you used for testing locally>'

# then proceed with 'source Submit_tf.sh <datasetname or list of datasetnames>'
