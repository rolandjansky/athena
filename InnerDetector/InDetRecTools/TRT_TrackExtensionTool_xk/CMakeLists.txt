# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TRT_TrackExtensionTool_xk )

# Component(s) in the package:
atlas_add_component( TRT_TrackExtensionTool_xk
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps AthenaPoolUtilities GaudiKernel InDetIdentifier InDetPrepRawData InDetRIO_OnTrack InDetRecToolInterfaces MagFieldConditions MagFieldElements StoreGateLib TRT_ReadoutGeometry TrkEventPrimitives TrkEventUtils TrkExInterfaces TrkGeometry TrkPatternParameters TrkPseudoMeasurementOnTrack TrkRIO_OnTrack TrkSegment TrkSurfaces TrkToolInterfaces TrkTrack )
