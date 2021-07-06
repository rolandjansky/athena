# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( SCT_ReadoutGeometry )

# External dependencies:
find_package( CLHEP )
find_package( GeoModel COMPONENTS GeoModelKernel )

# Component(s) in the package:
atlas_add_library(SCT_ReadoutGeometry
  src/*.cxx
  PUBLIC_HEADERS SCT_ReadoutGeometry
  INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
  DEFINITIONS ${CLHEP_DEFINITIONS}
  LINK_LIBRARIES ${CLHEP_LIBRARIES} ${GEOMODEL_LIBRARIES} AthenaKernel GeoPrimitives InDetIdentifier InDetReadoutGeometry TrkSurfaces
  PRIVATE_LINK_LIBRARIES AthenaBaseComps AthenaPoolUtilities Identifier StoreGateLib )
