# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( GeoAdaptors )

# External dependencies:
find_package( CLHEP )

# Component(s) in the package:
atlas_add_library( GeoAdaptors
                   src/*.cxx
                   PUBLIC_HEADERS GeoAdaptors
                   INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                   DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${CLHEP_LIBRARIES} CaloIdentifier CaloSimEvent GeoPrimitives HGTD_Identifier HGTD_ReadoutGeometry Identifier InDetIdentifier InDetReadoutGeometry PixelReadoutGeometryLib SCT_ReadoutGeometry TRT_ReadoutGeometry InDetSimEvent LArSimEvent MuonReadoutGeometry MuonPrepRawData MuonSimEvent CaloDetDescrLib StoreGateLib MuonIdHelpersLib )
