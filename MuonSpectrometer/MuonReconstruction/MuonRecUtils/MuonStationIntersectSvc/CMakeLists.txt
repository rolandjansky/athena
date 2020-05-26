# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name.
atlas_subdir( MuonStationIntersectSvc )

# Component(s) in the package.
atlas_add_library( MuonStationIntersectSvcLib
   MuonStationIntersectSvc/*.h src/*.cxx
   PUBLIC_HEADERS MuonStationIntersectSvc
   LINK_LIBRARIES AthenaBaseComps GeoPrimitives Identifier MuonIdHelpersLib
   PRIVATE_LINK_LIBRARIES GaudiKernel StoreGateLib GeoModelUtilities
   MuonReadoutGeometry TrkDriftCircleMath MuonCondData )

atlas_add_component( MuonStationIntersectSvc
   src/components/*.cxx
   LINK_LIBRARIES MuonStationIntersectSvcLib )
