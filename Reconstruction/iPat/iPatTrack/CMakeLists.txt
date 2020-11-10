# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( iPatTrack )

# Component(s) in the package:
atlas_add_library( iPatTrack
                   src/HitOnTrack.cxx
                   src/HitQuality.cxx
                   src/HitStatistics.cxx
                   src/HitStatus.cxx
                   src/Track.cxx
                   src/TrackEnvelope.cxx
                   src/TrackStatus.cxx
                   src/TruthAssociation.cxx
                   PUBLIC_HEADERS iPatTrack
                   LINK_LIBRARIES Identifier EventPrimitives iPatTrackParameters TrkParameters
                   PRIVATE_LINK_LIBRARIES TrkSurfaces TrkMaterialOnTrack TrkMeasurementBase TrkRIO_OnTrack TrkTrack TrkExUtils )

