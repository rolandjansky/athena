# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( PixelReadoutGeometry )

# External dependencies:
find_package( GeoModel COMPONENTS GeoModelKernel )

# Component(s) in the package:
atlas_add_library( PixelReadoutGeometry
  src/*.cxx
  PUBLIC_HEADERS PixelReadoutGeometry
  LINK_LIBRARIES ${GEOMODEL_LIBRARIES} AthenaKernel CxxUtils GeoPrimitives InDetIdentifier InDetReadoutGeometry ReadoutGeometryBase TrkDetElementBase TrkSurfaces
  PRIVATE_LINK_LIBRARIES AthenaBaseComps AthenaPoolUtilities DetDescrConditions Identifier StoreGateLib TrkEventPrimitives )
