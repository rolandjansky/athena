# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrkParticleBase )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( TrkParticleBase
                   src/*.cxx
                   PUBLIC_HEADERS TrkParticleBase
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES AthLinks AthContainers TrkEventPrimitives TrkParameters TrkTrack VxVertex
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} GaudiKernel TrkTrackSummary )

atlas_add_dictionary( TrkParticleBaseDict
                      TrkParticleBase/TrkParticleBaseDict.h
                      TrkParticleBase/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} AthLinks AthContainers TrkEventPrimitives TrkParameters TrkTrack VxVertex GaudiKernel TrkTrackSummary TrkParticleBase
                      ELEMENT_LINKS DataVector<Trk::TrackParticleBase> )

