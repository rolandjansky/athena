# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Declare the package name.
atlas_subdir( MuonStationIntersectCond )

# Component(s) in the package.
atlas_add_library( MuonStationIntersectCondLib
   MuonStationIntersectCond/*.h src/*.cxx
   PUBLIC_HEADERS MuonStationIntersectCond
   LINK_LIBRARIES AthenaBaseComps GeoPrimitives Identifier MuonIdHelpersLib TrkDriftCircleMath
   PRIVATE_LINK_LIBRARIES GaudiKernel StoreGateLib GeoModelUtilities
   MuonReadoutGeometry MuonCondData )

atlas_add_component( MuonStationIntersectCond
   src/components/*.cxx
   LINK_LIBRARIES MuonStationIntersectCondLib MuonReadoutGeometry )
