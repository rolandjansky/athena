# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#
# This file holds the compile and runtime environment configuration specific
# for the DetCommon release.
#

# This is a standalone project, so set the appropriate compile flags:
add_definitions( -DXAOD_STANDALONE )
add_definitions( -DTRIGCONF_STANDALONE )

# And the same variables for CMake as well:
set( XAOD_STANDALONE TRUE CACHE BOOL
   "Flag specifying that this is a standalone build" )

set( TRIGCONF_STANDALONE TRUE CACHE BOOL
   "Flag specifying that this is a standalone build of the trigger configuration packages" )

# Declare the module "found".
set( DetCommonReleaseEnvironment_FOUND TRUE )
