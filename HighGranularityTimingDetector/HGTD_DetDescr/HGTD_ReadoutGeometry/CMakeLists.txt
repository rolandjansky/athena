# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( HGTD_ReadoutGeometry )

# External dependencies:
find_package( CLHEP )
find_package( GeoModel COMPONENTS GeoPrimitives GeoModelKernel )

# Component(s) in the package:
atlas_add_library( HGTD_ReadoutGeometry
   HGTD_ReadoutGeometry/*.h src/*.cxx
   PUBLIC_HEADERS HGTD_ReadoutGeometry
   INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS} ${GEOMODEL_INCLUDE_DIRS}
   DEFINITIONS ${CLHEP_DEFINITIONS}
   LINK_LIBRARIES ${CLHEP_LIBRARIES} ${GEOMODEL_LIBRARIES} Identifier
   ReadoutGeometryBase TrkEventPrimitives HGTD_Identifier AthenaBaseComps
   TrkSurfaces
   PRIVATE_LINK_LIBRARIES AtlasDetDescr StoreGateLib GeoModelUtilities )
