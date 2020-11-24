# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( Particle )

# Component(s) in the package:
atlas_add_library( Particle
                   src/*.cxx
                   PUBLIC_HEADERS Particle
                   LINK_LIBRARIES AthLinks AthContainers Navigation AthenaKernel EventKernel FourMom GaudiKernel TrkEventPrimitives TrkParameters TrkParticleBase TrkTrack TrkTrackSummary VxVertex )

atlas_add_dictionary( ParticleDict
                      Particle/ParticleDict.h
                      Particle/selection.xml
                      LINK_LIBRARIES Particle
                      NAVIGABLES Rec::TrackParticleContainer )

