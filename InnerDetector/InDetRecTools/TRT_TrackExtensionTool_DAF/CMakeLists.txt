# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TRT_TrackExtensionTool_DAF )

# Component(s) in the package:
atlas_add_component( TRT_TrackExtensionTool_DAF
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps GaudiKernel InDetCompetingRIOsOnTrackToolLib InDetIdentifier InDetPrepRawData InDetRecToolInterfaces MagFieldConditions MagFieldElements TRT_ReadoutGeometry TrkEventUtils TrkExInterfaces TrkGeometry TrkMeasurementBase TrkParameters TrkToolInterfaces TrkTrack )
