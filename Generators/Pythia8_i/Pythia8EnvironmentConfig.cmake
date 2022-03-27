# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
#
# This module is used to set up the environment for Pythia8
# 
#

# Set the environment variable(s):
find_package( Pythia8 )
find_package( Lhapdf )

if( PYTHIA8_FOUND AND LHAPDF_FOUND )
  set( PYTHIA8ENVIRONMENT_ENVIRONMENT 
        FORCESET PYTHIA8VER ${PYTHIA8_LCGVERSION}
        FORCESET PY8PATH ${PYTHIA8_LCGROOT}
        FORCESET LHAPDFVER ${LHAPDF_LCGVERSION} )
endif()

# Silently declare the module found:
set( PYTHIA8ENVIRONMENT_FOUND TRUE )



