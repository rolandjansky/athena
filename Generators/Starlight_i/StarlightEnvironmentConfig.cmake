# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#
# This module is used to set up the environment for Starlight
# 
#

# Set the environment variable(s):
find_package( Starlight )

if( STARLIGHT_FOUND )
  set( STARLIGHTENVIRONMENT_ENVIRONMENT 
        FORCESET STARLIGHTVER ${STARLIGHT_LCGVERSION} ) 
endif()

# Silently declare the module found:
set( STARLIGHTENVIRONMENT_FOUND TRUE )


