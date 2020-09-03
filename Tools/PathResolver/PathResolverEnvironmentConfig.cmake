# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
#
# This module is used to set up the environment for PathResolver
# to use the calibration area correctly.
#

# Set the environment variable(s):
set( PATHRESOLVERENVIRONMENT_ENVIRONMENT
   APPEND CALIBPATH "/sw/DbData/GroupData"
   APPEND CALIBPATH "/cvmfs/atlas.cern.ch/repo/sw/database/GroupData"
   APPEND CALIBPATH "/eos/atlas/atlascerngroupdisk/asg-calib"
   APPEND CALIBPATH "/afs/cern.ch/atlas/www/GROUPS/DATABASE/GroupData"
   APPEND CALIBPATH "http//cern.ch/atlas-groupdata"
   APPEND CALIBPATH "http//atlas.web.cern.ch/Atlas/GROUPS/DATABASE/GroupData" )

# Silently declare the module found:
set( PATHRESOLVERENVIRONMENT_FOUND TRUE )
