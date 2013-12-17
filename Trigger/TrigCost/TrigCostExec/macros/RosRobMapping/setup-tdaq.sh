# change these parameters for the appropriate release and binary tag
rel=tdaq-02-00-03
cmttag=i686-slc5-gcc43-opt

# use afs installation at CERN, change path otherwise for different tdaq release installations
[ -z $TDAQ_INST_PATH ] && source /afs/cern.ch/atlas/project/tdaq/cmt/bin/cmtsetup.sh $rel $cmttag
