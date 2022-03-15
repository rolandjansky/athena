# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TGC_LinearSegmentMakerTool )

# Component(s) in the package:
atlas_add_component( TGC_LinearSegmentMakerTool
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps GaudiKernel MuonRecToolInterfaces StoreGateLib GeoPrimitives MuonReadoutGeometry MuonRIO_OnTrack MuonSegment MuonLinearSegmentMakerUtilities TrkSurfaces TrkEventPrimitives TrkRoad TrkExInterfaces TrkExUtils )

