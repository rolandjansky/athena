# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( ParticleEventAthenaPool )

atlas_add_library( ParticleEventAthenaPoolLib
                   ParticleEventAthenaPool/*.h
                   INTERFACE
                   PUBLIC_HEADERS ParticleEventAthenaPool
                   LINK_LIBRARIES AthenaPoolCnvSvcLib GaudiKernel ParticleEvent ParticleEventTPCnv StoreGateLib )

# Component(s) in the package:
atlas_add_poolcnv_library( ParticleEventAthenaPoolPoolCnv
                           src/*.cxx
                           FILES ParticleEvent/ParticleBaseContainer.h ParticleEvent/NeutrinoContainer.h ParticleEvent/CompositeParticleContainer.h ParticleEvent/ParticleShallowCloneContainer.h ParticleEvent/SelectedParticles.h ParticleEvent/TrackParticleAssocs.h ParticleEvent/INav4MomToTrackParticleAssocs.h
                           LINK_LIBRARIES ParticleEventAthenaPoolLib )
