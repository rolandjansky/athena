# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#
# This module is used to set up the environment for QGSJet
# 
#

# Set the environment variable(s):
find_package( Crmc )

if( CRMC_FOUND  )
  set( QGSJETENVIRONMENT_ENVIRONMENT 
        FORCESET QGSJETVER ${CRMC_LCGVERSION} )
endif()

# Silently declare the module found:
set( QGSJETENVIRONMENT_FOUND TRUE )


