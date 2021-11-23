# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#
# This module is used to set up the environment for Sherpa
# 
#

# Set the environment variable(s):
find_package( Sherpa )

if( SHERPA_FOUND )
  set( SHERPAENVIRONMENT_ENVIRONMENT 
        FORCESET SHERPAVER ${SHERPA_LCGVERSION} 
        FORCESET SHERPA_SHARE_PATH ${SHERPA_LCGROOT}/share/SHERPA-MC )
endif()

# Silently declare the module found:
set( SHERPAENVIRONMENT_FOUND TRUE )


