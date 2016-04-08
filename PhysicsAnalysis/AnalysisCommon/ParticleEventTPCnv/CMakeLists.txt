################################################################################
# Package: ParticleEventTPCnv
################################################################################

# Declare the package name:
atlas_subdir( ParticleEventTPCnv )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          AtlasTest/TestTools
                          Control/AthLinks
                          Control/DataModelAthenaPool
                          Database/AthenaPOOL/AthenaPoolCnvSvc
                          Event/EventCommonTPCnv
                          GaudiKernel
                          PhysicsAnalysis/AnalysisCommon/ParticleEvent
                          Reconstruction/Particle
                          PRIVATE
                          Control/AthAllocators
                          Control/AthenaKernel
                          Control/CxxUtils
                          Control/SGTools
                          Tracking/TrkEvent/VxVertex )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_tpcnv_library( ParticleEventTPCnv
                         src/*.cxx
                         PUBLIC_HEADERS ParticleEventTPCnv
                         INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                         LINK_LIBRARIES ${ROOT_LIBRARIES} TestTools AthLinks DataModelAthenaPoolLib AthenaPoolCnvSvcLib EventCommonTPCnv GaudiKernel ParticleEvent Particle AthAllocators AthenaKernel CxxUtils SGTools VxVertex )

atlas_add_dictionary( ParticleEventTPCnvDict
                      ParticleEventTPCnv/ParticleEventTPCnvDict.h
                      ParticleEventTPCnv/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} TestTools AthLinks DataModelAthenaPoolLib AthenaPoolCnvSvcLib EventCommonTPCnv GaudiKernel ParticleEvent Particle AthAllocators AthenaKernel CxxUtils SGTools VxVertex ParticleEventTPCnv )

atlas_add_test( CompositeParticleCnv_p1_test
                SOURCES
                test/CompositeParticleCnv_p1_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} TestTools AthLinks DataModelAthenaPoolLib AthenaPoolCnvSvcLib EventCommonTPCnv GaudiKernel ParticleEvent Particle AthAllocators AthenaKernel CxxUtils SGTools VxVertex ParticleEventTPCnv )

atlas_add_test( CompositeParticleContainerCnv_p1_test
                SOURCES
                test/CompositeParticleContainerCnv_p1_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} TestTools AthLinks DataModelAthenaPoolLib AthenaPoolCnvSvcLib EventCommonTPCnv GaudiKernel ParticleEvent Particle AthAllocators AthenaKernel CxxUtils SGTools VxVertex ParticleEventTPCnv )

atlas_add_test( INav4MomToTrackParticleAssocsCnv_p1_test
                SOURCES
                test/INav4MomToTrackParticleAssocsCnv_p1_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} TestTools AthLinks DataModelAthenaPoolLib AthenaPoolCnvSvcLib EventCommonTPCnv GaudiKernel ParticleEvent Particle AthAllocators AthenaKernel CxxUtils SGTools VxVertex ParticleEventTPCnv )

atlas_add_test( NeutrinoCnv_p1_test
                SOURCES
                test/NeutrinoCnv_p1_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} TestTools AthLinks DataModelAthenaPoolLib AthenaPoolCnvSvcLib EventCommonTPCnv GaudiKernel ParticleEvent Particle AthAllocators AthenaKernel CxxUtils SGTools VxVertex ParticleEventTPCnv )

atlas_add_test( NeutrinoCnv_p2_test
                SOURCES
                test/NeutrinoCnv_p2_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} TestTools AthLinks DataModelAthenaPoolLib AthenaPoolCnvSvcLib EventCommonTPCnv GaudiKernel ParticleEvent Particle AthAllocators AthenaKernel CxxUtils SGTools VxVertex ParticleEventTPCnv )

atlas_add_test( NeutrinoContainerCnv_p1_test
                SOURCES
                test/NeutrinoContainerCnv_p1_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} TestTools AthLinks DataModelAthenaPoolLib AthenaPoolCnvSvcLib EventCommonTPCnv GaudiKernel ParticleEvent Particle AthAllocators AthenaKernel CxxUtils SGTools VxVertex ParticleEventTPCnv )

atlas_add_test( NeutrinoContainerCnv_p2_test
                SOURCES
                test/NeutrinoContainerCnv_p2_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} TestTools AthLinks DataModelAthenaPoolLib AthenaPoolCnvSvcLib EventCommonTPCnv GaudiKernel ParticleEvent Particle AthAllocators AthenaKernel CxxUtils SGTools VxVertex ParticleEventTPCnv )

atlas_add_test( ParticleBaseCnv_p1_test
                SOURCES
                test/ParticleBaseCnv_p1_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} TestTools AthLinks DataModelAthenaPoolLib AthenaPoolCnvSvcLib EventCommonTPCnv GaudiKernel ParticleEvent Particle AthAllocators AthenaKernel CxxUtils SGTools VxVertex ParticleEventTPCnv )

atlas_add_test( ParticleBaseCnv_p2_test
                SOURCES
                test/ParticleBaseCnv_p2_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} TestTools AthLinks DataModelAthenaPoolLib AthenaPoolCnvSvcLib EventCommonTPCnv GaudiKernel ParticleEvent Particle AthAllocators AthenaKernel CxxUtils SGTools VxVertex ParticleEventTPCnv )

atlas_add_test( ParticleBaseLinksCnv_p1_test
                SOURCES
                test/ParticleBaseLinksCnv_p1_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} TestTools AthLinks DataModelAthenaPoolLib AthenaPoolCnvSvcLib EventCommonTPCnv GaudiKernel ParticleEvent Particle AthAllocators AthenaKernel CxxUtils SGTools VxVertex ParticleEventTPCnv )

atlas_add_test( ParticleShallowCloneCnv_p1_test
                SOURCES
                test/ParticleShallowCloneCnv_p1_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} TestTools AthLinks DataModelAthenaPoolLib AthenaPoolCnvSvcLib EventCommonTPCnv GaudiKernel ParticleEvent Particle AthAllocators AthenaKernel CxxUtils SGTools VxVertex ParticleEventTPCnv )

atlas_add_test( ParticleShallowCloneContainerCnv_p1_test
                SOURCES
                test/ParticleShallowCloneContainerCnv_p1_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} TestTools AthLinks DataModelAthenaPoolLib AthenaPoolCnvSvcLib EventCommonTPCnv GaudiKernel ParticleEvent Particle AthAllocators AthenaKernel CxxUtils SGTools VxVertex ParticleEventTPCnv )

atlas_add_test( SelectedParticlesCnv_p1_test
                SOURCES
                test/SelectedParticlesCnv_p1_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} TestTools AthLinks DataModelAthenaPoolLib AthenaPoolCnvSvcLib EventCommonTPCnv GaudiKernel ParticleEvent Particle AthAllocators AthenaKernel CxxUtils SGTools VxVertex ParticleEventTPCnv )

atlas_add_test( TrackParticleAssocsCnv_p1_test
                SOURCES
                test/TrackParticleAssocsCnv_p1_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} TestTools AthLinks DataModelAthenaPoolLib AthenaPoolCnvSvcLib EventCommonTPCnv GaudiKernel ParticleEvent Particle AthAllocators AthenaKernel CxxUtils SGTools VxVertex ParticleEventTPCnv )

