# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( InDetBeamSpotService )

# Possible extra dependencies:
set( extra_lib )
if( NOT SIMULATIONBASE )
   set( extra_lib VxVertex )
endif()

# External dependencies:
find_package( CORAL COMPONENTS CoralBase )

# Component(s) in the package:
atlas_add_library( InDetBeamSpotServiceLib
   InDetBeamSpotService/*.h
   INTERFACE
   PUBLIC_HEADERS InDetBeamSpotService
   LINK_LIBRARIES GaudiKernel GeoPrimitives ${extra_lib} )

atlas_add_component( InDetBeamSpotService
   src/*.h src/*.cxx src/components/*.cxx
   INCLUDE_DIRS ${CORAL_INCLUDE_DIRS}
   LINK_LIBRARIES ${CORAL_LIBRARIES} GeoPrimitives GaudiKernel AthenaBaseComps CxxUtils
   StoreGateLib AthenaPoolUtilities EventPrimitives InDetBeamSpotServiceLib
   ${extra_lib} )

atlas_add_dictionary( InDetBeamSpotServiceDict
   InDetBeamSpotService/IBeamCondSvc.h
   InDetBeamSpotService/selection.xml
   LINK_LIBRARIES InDetBeamSpotServiceLib )

# Install files from the package:
atlas_install_joboptions( share/*.py )
