# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#
# This module is used to set up the environment for Rivet
# 
#

# Set the environment variable(s):
find_package( Rivet )
find_package( YODA )

if( RIVET_FOUND AND YODA_FOUND )
  get_filename_component( _rivetPath "${RIVET_ROOT}" DIRECTORY )
  set( RIVETENVIRONMENT_ENVIRONMENT 
        FORCESET RIVETVER ${RIVET_VERSION} 
        FORCESET YODAVER ${YODA_VERSION}
        FORCESET RIVET_PATH ${_rivetPath}/${LCG_PLATFORM})
  unset( _rivetPath )
endif()

# Silently declare the module found:
set( RIVETENVIRONMENT_FOUND TRUE )


