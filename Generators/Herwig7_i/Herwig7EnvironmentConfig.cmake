# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#
# This module is used to set up the environment for Herwig7
# 
#

# Set the environment variable(s):
find_package( Herwig3 )
find_package( ThePEG )

if( HERWIG3_FOUND AND THEPEG_FOUND )
  set( HERWIG7ENVIRONMENT_ENVIRONMENT
        FORCESET HERWIG7_PATH ${HERWIG3_LCGROOT}
        FORCESET HERWIG7VER ${HERWIG3_LCGVERSION}
        FORCESET THEPEGVER ${THEPEG_LCGVERSION})
endif()

# Silently declare the module found:
set( HERWIG7ENVIRONMENT_FOUND TRUE )
