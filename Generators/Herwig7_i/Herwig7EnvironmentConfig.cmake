# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
#
# This module is used to set up the environment for Herwig7
# 
#

# Set the environment variable(s):
find_package( Herwig3 )

if( HERWIG3_FOUND )
  get_filename_component( _herwig3Path "${HERWIG3_ROOT}" DIRECTORY )
  set( HERWIG7ENVIRONMENT_ENVIRONMENT 
        FORCESET HERWIG7_PATH ${_herwig3Path}/${LCG_PLATFORM} )
  unset( _herwig3Path )
endif()

# Silently declare the module found:
set( HERWIG7ENVIRONMENT_FOUND TRUE )

