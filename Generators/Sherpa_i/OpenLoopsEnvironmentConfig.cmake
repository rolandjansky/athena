# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#
# This module is used to set up the environment for OpenLoops
# 
#

# Set the environment variable(s):
find_package( OpenLoops )

if( OPENLOOPS_FOUND )
  set( OPENLOOPSENVIRONMENT_ENVIRONMENT 
        FORCESET OPENLOOPSVER ${OPENLOOPS_LCGVERSION} 
        FORCESET OPENLOOPSPATH ${OPENLOOPS_LCGROOT} )
endif()

# Silently declare the module found:
set( OPENLOOPSENVIRONMENT_FOUND TRUE )



