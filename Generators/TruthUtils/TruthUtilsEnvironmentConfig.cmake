# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#
# This module is used to set up the environment for TruthUtils
# 
#

# Set the environment variable(s):
find_package( HEPUtils )
find_package( MCUtils )

if( HEPUTILS_FOUND AND MCUTILS_FOUND )
  set( TRUTHUTILSENVIRONMENT_ENVIRONMENT 
        FORCESET MCUTILSVER ${MCUTILS_LCGVERSION} 
        FORCESET HEPUTILSVER ${HEPUTILS_LCGVERSION} )
endif()

# Silently declare the module found:
set( TRUTHUTILSENVIRONMENT_FOUND TRUE )


