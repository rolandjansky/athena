# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( AFP_GeoModel )

# External dependencies:
find_package( CLHEP )
find_package( GeoModelCore )

# Component(s) in the package:
atlas_add_library( AFP_GeoModelLib
                   src/*.cxx
                   PUBLIC_HEADERS AFP_GeoModel
                   PRIVATE_INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS} ${GEOMODELCORE_INCLUDE_DIRS}
                   PRIVATE_DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${GEOMODELCORE_LIBRARIES} AFP_Geometry AthenaKernel GeoModelUtilities
                   PRIVATE_LINK_LIBRARIES ${CLHEP_LIBRARIES} AthenaPoolUtilities GaudiKernel GeoModelInterfaces GeoPrimitives StoreGateLib )

atlas_add_component( AFP_GeoModel
                     src/components/*.cxx
                     LINK_LIBRARIES AFP_GeoModelLib )
