# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( InDetAlignGeomTools )

# External dependencies:
find_package( ROOT COMPONENTS Core Geom )

# Component(s) in the package:
atlas_add_library( InDetAlignGeomToolsLib
   InDetAlignGeomTools/*.h
   INTERFACE
   PUBLIC_HEADERS InDetAlignGeomTools
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaBaseComps AthContainers
   TrkAlignEvent TrkAlignGenToolsLib TrkAlignInterfaces )

atlas_add_component( InDetAlignGeomTools
   src/*.cxx src/components/*.cxx
   LINK_LIBRARIES AthContainers GeoModelInterfaces GeoModelUtilities InDetAlignGeomToolsLib
   InDetIdentifier InDetReadoutGeometry PixelReadoutGeometryLib RDBAccessSvcLib SCT_ReadoutGeometry
   TRT_ReadoutGeometry TrkAlignEvent TrkAlignInterfaces TrkRIO_OnTrack )
