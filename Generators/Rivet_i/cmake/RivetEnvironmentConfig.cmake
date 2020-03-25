# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#
# This module is used to set up the runtime environment for Rivet.
#

# Packages needed for the environment variable setup.
find_package( Rivet )
find_package( YODA )

# Set the environment variable(s).
set( RivetEnvironment_ENVIRONMENT 
   FORCESET RIVETVER ${RIVET_LCGVERSION} 
   FORCESET YODAVER ${YODA_LCGVERSION}
   FORCESET RIVET_PATH ${RIVET_LCGROOT} )

# Base the module's "found status" on whether we found the generators.
if( RIVET_FOUND AND YODA_FOUND )
   set( RivetEnvironment_FOUND TRUE )
else()
   set( RivetEnvironment_FOUND FALSE )
endif()
