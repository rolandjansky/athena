# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrkTrack )

# Component(s) in the package:
atlas_add_library( TrkTrack
                   src/*.cxx
                   PUBLIC_HEADERS TrkTrack
                   LINK_LIBRARIES AthContainers AthLinks AthenaKernel CxxUtils Identifier TrkEventPrimitives TrkMaterialOnTrack TrkMeasurementBase TrkNeutralParameters TrkParameters TrkTrackLink
                   PRIVATE_LINK_LIBRARIES GaudiKernel TrkSurfaces TrkTrackSummary )

atlas_add_dictionary( TrkTrackDict
                      TrkTrack/TrkTrackDict.h
                      TrkTrack/selection.xml
                      LINK_LIBRARIES AthLinks Identifier TrkTrack
                      ELEMENT_LINKS TrackCollection )

