# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( SCT_ReadoutGeometry )

# External dependencies:
find_package( CLHEP )
find_package( GeoModelCore )

# Component(s) in the package:
atlas_add_library(SCT_ReadoutGeometry
  src/*.cxx
  PUBLIC_HEADERS SCT_ReadoutGeometry
  INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
  DEFINITIONS ${CLHEP_DEFINITIONS}
  LINK_LIBRARIES ${CLHEP_LIBRARIES} ${GEOMODELCORE_LIBRARIES} AthenaKernel GeoPrimitives InDetIdentifier InDetReadoutGeometry TrkSurfaces
  PRIVATE_LINK_LIBRARIES AthenaBaseComps AthenaPoolUtilities Identifier StoreGateLib )
