# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: PathResolverEnvironmentConfig.cmake 723387 2016-02-11 14:32:28Z krasznaa $
#
# This module is used to set up the environment for PathResolver
# to use the calibration area correctly.
#

# Set the environment variable(s):
set( PATHRESOLVERENVIRONMENT_ENVIRONMENT
   APPEND CALIBPATH "/sw/DbData/GroupData"
   APPEND CALIBPATH "/cvmfs/atlas.cern.ch/repo/sw/database/GroupData"
   APPEND CALIBPATH "/afs/cern.ch/atlas/www/GROUPS/DATABASE/GroupData"
   APPEND CALIBPATH "http//atlas.web.cern.ch/Atlas/GROUPS/DATABASE/GroupData" )

# Silently declare the module found:
set( PATHRESOLVERENVIRONMENT_FOUND TRUE )
