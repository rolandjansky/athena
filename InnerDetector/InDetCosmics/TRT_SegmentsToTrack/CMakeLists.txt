# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TRT_SegmentsToTrack )

# Component(s) in the package:
atlas_add_component( TRT_SegmentsToTrack
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthContainers AthenaBaseComps GaudiKernel AtlasDetDescr GeoPrimitives Identifier EventPrimitives InDetIdentifier InDetPrepRawData InDetRIO_OnTrack StoreGateLib TrkSurfaces TrkEventUtils TrkParameters TrkPseudoMeasurementOnTrack TrkRIO_OnTrack TrkSegment TrkTrack TrkTruthData TrkExInterfaces TrkFitterInterfaces TrkToolInterfaces )
