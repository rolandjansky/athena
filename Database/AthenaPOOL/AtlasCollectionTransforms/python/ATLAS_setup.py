# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#-----------------------------------------------------
#
# This file should return a setup cmd which can be 
#   executed at the linux prompt to setup the correct 
#   Atlas environment. It has been tested on lxplus.
#
# Jack Cranshaw 06/08/08
#
# 
#-----------------------------------------------------

import sys, os

def atlas_setup_14(workarea,release): 

  # some global settings

  # release-related settings
  os.environ['RELEASE']  = release
  os.environ['ATLAS_ROOT'] = '/afs/cern.ch/atlas'

  setup_cmd = common(workarea,release)

  return setup_cmd

def common(workarea,release):

  cmd = "export SITEROOT=/afs/cern.ch ; "
  cmd += "source ${SITEROOT}/sw/contrib/CMT/v1r20p20080222/mgr/setup.sh ; "
  if (workarea != ""): 
    cmd += "export CMTPATH=" + workarea + ":${CMTPATH} ; "
  cmd += "echo \"### CMTCONFIG = $CMTCONFIG\" ; "
  cmd += "echo \"### CMTPATH   = $CMTPATH\" ; "
  cmd += "echo \"### sourcing $ATLAS_ROOT/software/builds/AtlasOffline/$RELEASE/AtlasOfflineRunTime/cmt/setup.sh\" ; "
  #cmd += "source $ATLAS_ROOT/software/builds/AtlasOffline/$RELEASE/AtlasOfflineRunTime/cmt/setup.sh \"\" ; "
  #cmd += "source $HOME/cmthome/setup.sh -tag="+release+",AtlasProduction,releases,runtime,32,opt ; "
  cmd += "source $ATLAS_ROOT/software/builds/AtlasOffline/$RELEASE/AtlasOfflineRunTime/cmt/setup.sh -tag=32,opt ; "

  if (workarea != ""): 
    cmd += "source " + workarea + "/Database/AthenaPOOL/AtlasCollectionTransforms/cmt/setup.sh ; "

  cmd += "export CORAL_AUTH_PATH=$DATADIR ; "


  return cmd
