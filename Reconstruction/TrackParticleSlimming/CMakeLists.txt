# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrackParticleSlimming )

# Component(s) in the package:
atlas_add_library( TrackParticleSlimming
                   src/*.cxx
                   PUBLIC_HEADERS TrackParticleSlimming
                   LINK_LIBRARIES AthenaKernel Particle TrkEventPrimitives TrkParametersBase TrkTrack TrkTrackSummary )

