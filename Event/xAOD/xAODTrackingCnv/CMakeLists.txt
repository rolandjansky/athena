# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( xAODTrackingCnv )

# Component(s) in the package. Built in a much more lightweight fashion for
# AthAnalysis.
if( XAOD_ANALYSIS )

   atlas_add_library( xAODTrackingCnvLib
      xAODTrackingCnv/ITrackParticleCompressorTool.h
      INTERFACE
      PUBLIC_HEADERS xAODTrackingCnv
      LINK_LIBRARIES xAODTracking GaudiKernel )

   atlas_add_component( xAODTrackingCnv
      src/TrackParticleCompressorTool.* src/components/*.cxx
      LINK_LIBRARIES AthenaBaseComps CxxUtils xAODTrackingCnvLib )

else()

   atlas_add_library( xAODTrackingCnvLib
      xAODTrackingCnv/*.h
      INTERFACE
      PUBLIC_HEADERS xAODTrackingCnv
      LINK_LIBRARIES xAODTracking TrkTrack GaudiKernel )

   atlas_add_component( xAODTrackingCnv
      src/*.h src/*.cxx src/components/*.cxx
      LINK_LIBRARIES xAODTracking TrkTrack AthenaBaseComps AthenaKernel
      EventPrimitives GaudiKernel GeneratorObjects MCTruthClassifierLib Particle
      ParticleTruth TrkLinks TrkParticleBase TrkTruthData VxVertex
      TrkToolInterfaces xAODCore xAODTrackingCnvLib
      PRIVATE_LINK_LIBRARIES CxxUtils )

endif()
