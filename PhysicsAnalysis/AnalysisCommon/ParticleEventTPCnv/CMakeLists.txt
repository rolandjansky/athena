# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( ParticleEventTPCnv )

# Component(s) in the package:
atlas_add_tpcnv_library( ParticleEventTPCnv
                         src/*.cxx
                         PUBLIC_HEADERS ParticleEventTPCnv
                         LINK_LIBRARIES AthLinks AthenaPoolCnvSvcLib DataModelAthenaPoolLib EventCommonTPCnv GaudiKernel Particle ParticleEvent
                         PRIVATE_LINK_LIBRARIES AthAllocators AthenaKernel VxVertex )

atlas_add_dictionary( ParticleEventTPCnvDict
                      ParticleEventTPCnv/ParticleEventTPCnvDict.h
                      ParticleEventTPCnv/selection.xml
                      LINK_LIBRARIES ParticleEventTPCnv )

# Tests in the package:
atlas_add_test( CompositeParticleCnv_p1_test
                SOURCES
                test/CompositeParticleCnv_p1_test.cxx
                LINK_LIBRARIES AthenaPoolCnvSvcLib GaudiKernel ParticleEvent ParticleEventTPCnv SGTools TestTools CxxUtils)

atlas_add_test( CompositeParticleContainerCnv_p1_test
                SOURCES
                test/CompositeParticleContainerCnv_p1_test.cxx
                LINK_LIBRARIES AthAllocators GaudiKernel ParticleEvent ParticleEventTPCnv SGTools TestTools CxxUtils)

atlas_add_test( INav4MomToTrackParticleAssocsCnv_p1_test
                SOURCES
                test/INav4MomToTrackParticleAssocsCnv_p1_test.cxx
                LINK_LIBRARIES GaudiKernel ParticleEvent ParticleEventTPCnv SGTools TestTools CxxUtils)

atlas_add_test( NeutrinoCnv_p1_test
                SOURCES
                test/NeutrinoCnv_p1_test.cxx
                LINK_LIBRARIES AthenaPoolCnvSvcLib GaudiKernel ParticleEvent ParticleEventTPCnv SGTools TestTools CxxUtils)

atlas_add_test( NeutrinoCnv_p2_test
                SOURCES
                test/NeutrinoCnv_p2_test.cxx
                LINK_LIBRARIES AthenaPoolCnvSvcLib GaudiKernel ParticleEvent ParticleEventTPCnv SGTools TestTools CxxUtils)

atlas_add_test( NeutrinoContainerCnv_p1_test
                SOURCES
                test/NeutrinoContainerCnv_p1_test.cxx
                LINK_LIBRARIES GaudiKernel ParticleEvent ParticleEventTPCnv SGTools TestTools CxxUtils)

atlas_add_test( NeutrinoContainerCnv_p2_test
                SOURCES
                test/NeutrinoContainerCnv_p2_test.cxx
                LINK_LIBRARIES GaudiKernel ParticleEvent ParticleEventTPCnv SGTools TestTools CxxUtils)

atlas_add_test( ParticleBaseCnv_p1_test
                SOURCES
                test/ParticleBaseCnv_p1_test.cxx
                LINK_LIBRARIES GaudiKernel ParticleEvent ParticleEventTPCnv SGTools TestTools CxxUtils)

atlas_add_test( ParticleBaseCnv_p2_test
                SOURCES
                test/ParticleBaseCnv_p2_test.cxx
                LINK_LIBRARIES GaudiKernel ParticleEvent ParticleEventTPCnv SGTools TestTools CxxUtils)

atlas_add_test( ParticleBaseLinksCnv_p1_test
                SOURCES
                test/ParticleBaseLinksCnv_p1_test.cxx
                LINK_LIBRARIES GaudiKernel ParticleEvent ParticleEventTPCnv SGTools TestTools CxxUtils)

atlas_add_test( ParticleShallowCloneCnv_p1_test
                SOURCES
                test/ParticleShallowCloneCnv_p1_test.cxx
                LINK_LIBRARIES GaudiKernel ParticleEvent ParticleEventTPCnv SGTools TestTools CxxUtils)

atlas_add_test( ParticleShallowCloneContainerCnv_p1_test
                SOURCES
                test/ParticleShallowCloneContainerCnv_p1_test.cxx
                LINK_LIBRARIES AthenaPoolCnvSvcLib AthAllocators GaudiKernel ParticleEvent ParticleEventTPCnv SGTools TestTools CxxUtils)

atlas_add_test( SelectedParticlesCnv_p1_test
                SOURCES
                test/SelectedParticlesCnv_p1_test.cxx
                LINK_LIBRARIES GaudiKernel ParticleEvent ParticleEventTPCnv TestTools CxxUtils)

atlas_add_test( TrackParticleAssocsCnv_p1_test
                SOURCES
                test/TrackParticleAssocsCnv_p1_test.cxx
                LINK_LIBRARIES GaudiKernel ParticleEvent ParticleEventTPCnv SGTools TestTools CxxUtils)
