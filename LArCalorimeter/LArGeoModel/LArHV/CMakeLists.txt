# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( LArHV )

# Possible extra dependencies:
set( extra_lib )
if( NOT SIMULATIONBASE AND NOT GENERATIONBASE )
   set( extra_lib LArRecConditions )
endif()

# External dependencies:
find_package( GeoModel COMPONENTS GeoModelKernel )

# Component(s) in the package:
atlas_add_library( LArHV
                   src/*.cxx
                   PUBLIC_HEADERS LArHV
                   INCLUDE_DIRS ${GEOMODEL_INCLUDE_DIRS}
                   LINK_LIBRARIES ${GEOMODEL_LIBRARIES} AthenaKernel
                   PRIVATE_LINK_LIBRARIES AthenaPoolUtilities GaudiKernel Identifier LArCablingLib LArIdentifier StoreGateLib ${extra_lib} )
