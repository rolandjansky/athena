# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#
# This module is used to set up the environment for Epos
# 
#

# Set the environment variable(s):
find_package( CRMC )

if( CRMC_FOUND  )
  set( EPOSENVIRONMENT_ENVIRONMENT 
        FORCESET EPOSVER ${CRMC_VERSION} )
endif()

# Silently declare the module found:
set( EPOSENVIRONMENT_FOUND TRUE )


