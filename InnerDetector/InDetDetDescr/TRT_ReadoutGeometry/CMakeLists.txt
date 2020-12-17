# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TRT_ReadoutGeometry )

# External dependencies:
find_package( CLHEP )
find_package( GeoModelCore )

# Component(s) in the package:
atlas_add_library( TRT_ReadoutGeometry
  src/*.cxx
  PUBLIC_HEADERS TRT_ReadoutGeometry
  INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
  DEFINITIONS ${CLHEP_DEFINITIONS}
  LINK_LIBRARIES ${CLHEP_LIBRARIES} ${GEOMODELCORE_LIBRARIES} AthenaBaseComps AthenaKernel CxxUtils EventPrimitives 
  GeoPrimitives TrkSurfaces Identifier InDetIdentifier InDetReadoutGeometry ReadoutGeometryBase TrkDetElementBase
  PRIVATE_LINK_LIBRARIES DetDescrConditions GaudiKernel GeoModelUtilities StoreGateLib TRT_ConditionsData)
