# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrackParticleTPCnv )

# Component(s) in the package:
atlas_add_tpcnv_library( TrackParticleTPCnv
                         src/*.cxx
                         src/Particle/*.cxx
                         PUBLIC_HEADERS TrackParticleTPCnv
                         LINK_LIBRARIES AthLinks DataModelAthenaPoolLib AthenaPoolCnvSvcLib AthenaPoolUtilities Particle TrkParameters TrkParticleBase TrkTrack VxVertex TrkEventTPCnv AthenaKernel GeoPrimitives )

atlas_add_dictionary( TrackParticleTPCnvDict
                      TrackParticleTPCnv/TrackParticleTPCnvDict.h
                      TrackParticleTPCnv/selection.xml
                      LINK_LIBRARIES TrackParticleTPCnv )
