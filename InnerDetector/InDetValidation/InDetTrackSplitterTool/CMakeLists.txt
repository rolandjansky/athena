# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( InDetTrackSplitterTool )

atlas_add_library( InDetTrackSplitterToolLib
                   CInDetTrackSplitterTool/*.h
                   INTERFACE
                   PUBLIC_HEADERS InDetTrackSplitterTool
                   LINK_LIBRARIES GaudiKernel TrkTrack AthenaBaseComps )

# Component(s) in the package:
atlas_add_component( InDetTrackSplitterTool
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES InDetTrackSplitterToolLib GaudiKernel GeoPrimitives EventPrimitives InDetIdentifier InDetRIO_OnTrack TrkSurfaces TrkCompetingRIOsOnTrack TrkEventPrimitives TrkMeasurementBase TrkPseudoMeasurementOnTrack TrkFitterInterfaces )
