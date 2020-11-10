# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TRT_SeededTrackFinder )

# Component(s) in the package:
atlas_add_component( TRT_SeededTrackFinder
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps BeamSpotConditionsData CxxUtils GaudiKernel IRegionSelector InDetRIO_OnTrack InDetRecToolInterfaces RoiDescriptor SiSPSeededTrackFinderData TrkCaloClusterROI TrkEventPrimitives TrkEventUtils TrkExInterfaces TrkFitterInterfaces TrkPseudoMeasurementOnTrack TrkSegment TrkToolInterfaces TrkTrack )
