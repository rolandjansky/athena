# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#
# This module is used to set up the environment for EvtGen
# 
#

# Set the environment variable(s):
find_package( EvtGen )

if( EVTGEN_FOUND )
  set( EVTGENENVIRONMENT_ENVIRONMENT 
        FORCESET EVTGENVER ${EVTGEN_LCGVERSION} )
endif()

# Silently declare the module found:
set( EVTGENENVIRONMENT_FOUND TRUE )



