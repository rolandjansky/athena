# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#
# This module is used to set up the environment for TheP8I
# 
#

# Set the environment variable(s):
find_package( TheP8I )

if( THEP8I_FOUND )
  set( THEP8IENVIRONMENT_ENVIRONMENT 
        FORCESET THEP8IVER ${THEP8I_LCGVERSION} 
        FORCESET THEP8I_PATH ${THEP8I_LCGROOT} )
endif()

# Silently declare the module found:
set( THEP8IENVIRONMENT_FOUND TRUE )



