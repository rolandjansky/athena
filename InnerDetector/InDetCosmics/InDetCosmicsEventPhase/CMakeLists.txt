# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( InDetCosmicsEventPhase )

# Component(s) in the package:
atlas_add_component( InDetCosmicsEventPhase
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps GaudiKernel TRT_ConditionsServicesLib InDetRIO_OnTrack Particle StoreGateLib TrkParticleBase TrkSegment TrkSpaceTimePoint TrkTrack TrkFitterInterfaces TrkToolInterfaces CommissionEvent xAODEventInfo TRT_ConditionsData InDetIdentifier TrkTrackSummary )
