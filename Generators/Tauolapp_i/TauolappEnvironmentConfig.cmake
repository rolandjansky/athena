# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#
# This module is used to set up the environment for Tauolapp
# 
#

# Set the environment variable(s):
find_package( Tauolapp )

if( TAUOLAPP_FOUND )
  set( TAUOLAPPENVIRONMENT_ENVIRONMENT 
        FORCESET TAUOLAPPVER ${TAUOLAPP_LCGVERSION})
endif()

# Silently declare the module found:
set( TAUOLAPPENVIRONMENT_FOUND TRUE )


