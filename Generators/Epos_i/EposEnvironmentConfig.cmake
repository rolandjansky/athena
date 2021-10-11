# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#
# This module is used to set up the environment for Epos
# 
#

# Set the environment variable(s):
find_package( Crmc )

if( CRMC_FOUND  )
  set( EPOSENVIRONMENT_ENVIRONMENT 
        FORCESET EPOSVER ${CRMC_LCGVERSION} )
endif()

# Silently declare the module found:
set( EPOSENVIRONMENT_FOUND TRUE )


