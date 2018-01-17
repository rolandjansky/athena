################################################################################
# Package: TrigMuonEventTPCnv
################################################################################

# Declare the package name:
atlas_subdir( TrigMuonEventTPCnv )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthLinks
                          Control/DataModelAthenaPool
                          Database/AthenaPOOL/AthenaPoolCnvSvc
                          Database/AthenaPOOL/AthenaPoolUtilities
                          Event/EventCommonTPCnv
                          Event/FourMom
                          Reconstruction/Particle
                          Trigger/TrigEvent/TrigInDetEventTPCnv
                          Trigger/TrigEvent/TrigMuonEvent
                          PRIVATE
                          AtlasTest/TestTools
                          Control/AthenaKernel
                          Control/CxxUtils
                          Control/SGTools
                          GaudiKernel )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_tpcnv_library( TrigMuonEventTPCnv
                         src/*.cxx
                         PUBLIC_HEADERS TrigMuonEventTPCnv
                         PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                         LINK_LIBRARIES ${ROOT_LIBRARIES} AthLinks DataModelAthenaPoolLib AthenaPoolCnvSvcLib AthenaPoolUtilities EventCommonTPCnv FourMom Particle TrigInDetEventTPCnv TrigMuonEvent TestTools AthenaKernel CxxUtils SGTools GaudiKernel )

atlas_add_dictionary( TrigMuonEventTPCnvDict
                      TrigMuonEventTPCnv/TrigMuonEventTPCnvDict.h
                      TrigMuonEventTPCnv/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} AthLinks DataModelAthenaPoolLib AthenaPoolCnvSvcLib AthenaPoolUtilities EventCommonTPCnv FourMom Particle TrigInDetEventTPCnv TrigMuonEvent TestTools AthenaKernel CxxUtils SGTools GaudiKernel TrigMuonEventTPCnv )

atlas_add_dictionary( OLD_TrigMuonEventTPCnvDict
                      TrigMuonEventTPCnv/TrigMuonEventTPCnvDict.h
                      TrigMuonEventTPCnv/OLD_selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} AthLinks DataModelAthenaPoolLib AthenaPoolCnvSvcLib AthenaPoolUtilities EventCommonTPCnv FourMom Particle TrigInDetEventTPCnv TrigMuonEvent TestTools AthenaKernel CxxUtils SGTools GaudiKernel TrigMuonEventTPCnv )

atlas_add_test( CombinedMuonFeatureCnv_p2_test
                SOURCES
                test/CombinedMuonFeatureCnv_p2_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} AthLinks DataModelAthenaPoolLib AthenaPoolCnvSvcLib AthenaPoolUtilities EventCommonTPCnv FourMom Particle TrigInDetEventTPCnv TrigMuonEvent TestTools AthenaKernel CxxUtils SGTools GaudiKernel TrigMuonEventTPCnv )

atlas_add_test( CombinedMuonFeatureCnv_p3_test
                SOURCES
                test/CombinedMuonFeatureCnv_p3_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} AthLinks DataModelAthenaPoolLib AthenaPoolCnvSvcLib AthenaPoolUtilities EventCommonTPCnv FourMom Particle TrigInDetEventTPCnv TrigMuonEvent TestTools AthenaKernel CxxUtils SGTools GaudiKernel TrigMuonEventTPCnv )

atlas_add_test( CombinedMuonFeatureCnv_p4_test
                SOURCES
                test/CombinedMuonFeatureCnv_p4_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} AthLinks DataModelAthenaPoolLib AthenaPoolCnvSvcLib AthenaPoolUtilities EventCommonTPCnv FourMom Particle TrigInDetEventTPCnv TrigMuonEvent TestTools AthenaKernel CxxUtils SGTools GaudiKernel TrigMuonEventTPCnv )

atlas_add_test( IsoMuonFeatureCnv_p3_test
                SOURCES
                test/IsoMuonFeatureCnv_p3_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} AthLinks DataModelAthenaPoolLib AthenaPoolCnvSvcLib AthenaPoolUtilities EventCommonTPCnv FourMom Particle TrigInDetEventTPCnv TrigMuonEvent TestTools AthenaKernel CxxUtils SGTools GaudiKernel TrigMuonEventTPCnv )

atlas_add_test( MuonFeatureCnv_p1_test
                SOURCES
                test/MuonFeatureCnv_p1_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} AthLinks DataModelAthenaPoolLib AthenaPoolCnvSvcLib AthenaPoolUtilities EventCommonTPCnv FourMom Particle TrigInDetEventTPCnv TrigMuonEvent TestTools AthenaKernel CxxUtils SGTools GaudiKernel TrigMuonEventTPCnv )

atlas_add_test( MuonFeatureCnv_p2_test
                SOURCES
                test/MuonFeatureCnv_p2_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} AthLinks DataModelAthenaPoolLib AthenaPoolCnvSvcLib AthenaPoolUtilities EventCommonTPCnv FourMom Particle TrigInDetEventTPCnv TrigMuonEvent TestTools AthenaKernel CxxUtils SGTools GaudiKernel TrigMuonEventTPCnv )

atlas_add_test( MuonFeatureCnv_p3_test
                SOURCES
                test/MuonFeatureCnv_p3_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} AthLinks DataModelAthenaPoolLib AthenaPoolCnvSvcLib AthenaPoolUtilities EventCommonTPCnv FourMom Particle TrigInDetEventTPCnv TrigMuonEvent TestTools AthenaKernel CxxUtils SGTools GaudiKernel TrigMuonEventTPCnv )

atlas_add_test( MuonFeatureDetailsCnv_p1_test
                SOURCES
                test/MuonFeatureDetailsCnv_p1_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} AthLinks DataModelAthenaPoolLib AthenaPoolCnvSvcLib AthenaPoolUtilities EventCommonTPCnv FourMom Particle TrigInDetEventTPCnv TrigMuonEvent TestTools AthenaKernel CxxUtils SGTools GaudiKernel TrigMuonEventTPCnv )

atlas_add_test( MuonFeatureDetailsCnv_p2_test
                SOURCES
                test/MuonFeatureDetailsCnv_p2_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} AthLinks DataModelAthenaPoolLib AthenaPoolCnvSvcLib AthenaPoolUtilities EventCommonTPCnv FourMom Particle TrigInDetEventTPCnv TrigMuonEvent TestTools AthenaKernel CxxUtils SGTools GaudiKernel TrigMuonEventTPCnv )

atlas_add_test( TileMuFeatureCnv_p1_test
                SOURCES
                test/TileMuFeatureCnv_p1_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} AthLinks DataModelAthenaPoolLib AthenaPoolCnvSvcLib AthenaPoolUtilities EventCommonTPCnv FourMom Particle TrigInDetEventTPCnv TrigMuonEvent TestTools AthenaKernel CxxUtils SGTools GaudiKernel TrigMuonEventTPCnv )

atlas_add_test( TileMuFeatureCnv_p2_test
                SOURCES
                test/TileMuFeatureCnv_p2_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} AthLinks DataModelAthenaPoolLib AthenaPoolCnvSvcLib AthenaPoolUtilities EventCommonTPCnv FourMom Particle TrigInDetEventTPCnv TrigMuonEvent TestTools AthenaKernel CxxUtils SGTools GaudiKernel TrigMuonEventTPCnv )

atlas_add_test( TileTrackMuFeatureCnv_p2_test
                SOURCES
                test/TileTrackMuFeatureCnv_p2_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} AthLinks DataModelAthenaPoolLib AthenaPoolCnvSvcLib AthenaPoolUtilities EventCommonTPCnv FourMom Particle TrigInDetEventTPCnv TrigMuonEvent TestTools AthenaKernel CxxUtils SGTools GaudiKernel TrigMuonEventTPCnv )

atlas_add_test( TileTrackMuFeatureCnv_p3_test
                SOURCES
                test/TileTrackMuFeatureCnv_p3_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} AthLinks DataModelAthenaPoolLib AthenaPoolCnvSvcLib AthenaPoolUtilities EventCommonTPCnv FourMom Particle TrigInDetEventTPCnv TrigMuonEvent TestTools AthenaKernel CxxUtils SGTools GaudiKernel TrigMuonEventTPCnv )

atlas_add_test( TrigMuonClusterFeatureCnv_p1_test
                SOURCES
                test/TrigMuonClusterFeatureCnv_p1_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} AthLinks DataModelAthenaPoolLib AthenaPoolCnvSvcLib AthenaPoolUtilities EventCommonTPCnv FourMom Particle TrigInDetEventTPCnv TrigMuonEvent TestTools AthenaKernel CxxUtils SGTools GaudiKernel TrigMuonEventTPCnv )

atlas_add_test( TrigMuonEFCbTrackCnv_p5_test
                SOURCES
                test/TrigMuonEFCbTrackCnv_p5_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} AthLinks DataModelAthenaPoolLib AthenaPoolCnvSvcLib AthenaPoolUtilities EventCommonTPCnv FourMom Particle TrigInDetEventTPCnv TrigMuonEvent TestTools AthenaKernel CxxUtils SGTools GaudiKernel TrigMuonEventTPCnv )

atlas_add_test( TrigMuonEFCbTrackCnv_p6_test
                SOURCES
                test/TrigMuonEFCbTrackCnv_p6_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} AthLinks DataModelAthenaPoolLib AthenaPoolCnvSvcLib AthenaPoolUtilities EventCommonTPCnv FourMom Particle TrigInDetEventTPCnv TrigMuonEvent TestTools AthenaKernel CxxUtils SGTools GaudiKernel TrigMuonEventTPCnv )

atlas_add_test( TrigMuonEFCbTrackCnv_p7_test
                SOURCES
                test/TrigMuonEFCbTrackCnv_p7_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} AthLinks DataModelAthenaPoolLib AthenaPoolCnvSvcLib AthenaPoolUtilities EventCommonTPCnv FourMom Particle TrigInDetEventTPCnv TrigMuonEvent TestTools AthenaKernel CxxUtils SGTools GaudiKernel TrigMuonEventTPCnv )

atlas_add_test( TrigMuonEFCnv_p1_test
                SOURCES
                test/TrigMuonEFCnv_p1_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} AthLinks DataModelAthenaPoolLib AthenaPoolCnvSvcLib AthenaPoolUtilities EventCommonTPCnv FourMom Particle TrigInDetEventTPCnv TrigMuonEvent TestTools AthenaKernel CxxUtils SGTools GaudiKernel TrigMuonEventTPCnv )

atlas_add_test( TrigMuonEFCnv_p2_test
                SOURCES
                test/TrigMuonEFCnv_p2_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} AthLinks DataModelAthenaPoolLib AthenaPoolCnvSvcLib AthenaPoolUtilities EventCommonTPCnv FourMom Particle TrigInDetEventTPCnv TrigMuonEvent TestTools AthenaKernel CxxUtils SGTools GaudiKernel TrigMuonEventTPCnv )

atlas_add_test( TrigMuonEFInfoCnv_p4_test
                SOURCES
                test/TrigMuonEFInfoCnv_p4_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} AthLinks DataModelAthenaPoolLib AthenaPoolCnvSvcLib AthenaPoolUtilities EventCommonTPCnv FourMom Particle TrigInDetEventTPCnv TrigMuonEvent TestTools AthenaKernel CxxUtils SGTools GaudiKernel TrigMuonEventTPCnv )

atlas_add_test( TrigMuonEFInfoCnv_p5_test
                SOURCES
                test/TrigMuonEFInfoCnv_p5_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} AthLinks DataModelAthenaPoolLib AthenaPoolCnvSvcLib AthenaPoolUtilities EventCommonTPCnv FourMom Particle TrigInDetEventTPCnv TrigMuonEvent TestTools AthenaKernel CxxUtils SGTools GaudiKernel TrigMuonEventTPCnv )

atlas_add_test( TrigMuonEFInfoTrackCnv_p1_test
                SOURCES
                test/TrigMuonEFInfoTrackCnv_p1_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} AthLinks DataModelAthenaPoolLib AthenaPoolCnvSvcLib AthenaPoolUtilities EventCommonTPCnv FourMom Particle TrigInDetEventTPCnv TrigMuonEvent TestTools AthenaKernel CxxUtils SGTools GaudiKernel TrigMuonEventTPCnv )

atlas_add_test( TrigMuonEFIsolationCnv_p1_test
                SOURCES
                test/TrigMuonEFIsolationCnv_p1_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} AthLinks DataModelAthenaPoolLib AthenaPoolCnvSvcLib AthenaPoolUtilities EventCommonTPCnv FourMom Particle TrigInDetEventTPCnv TrigMuonEvent TestTools AthenaKernel CxxUtils SGTools GaudiKernel TrigMuonEventTPCnv )

atlas_add_test( TrigMuonEFIsolationCnv_p2_test
                SOURCES
                test/TrigMuonEFIsolationCnv_p2_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} AthLinks DataModelAthenaPoolLib AthenaPoolCnvSvcLib AthenaPoolUtilities EventCommonTPCnv FourMom Particle TrigInDetEventTPCnv TrigMuonEvent TestTools AthenaKernel CxxUtils SGTools GaudiKernel TrigMuonEventTPCnv )

atlas_add_test( TrigMuonEFTrackCnv_p5_test
                SOURCES
                test/TrigMuonEFTrackCnv_p5_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} AthLinks DataModelAthenaPoolLib AthenaPoolCnvSvcLib AthenaPoolUtilities EventCommonTPCnv FourMom Particle TrigInDetEventTPCnv TrigMuonEvent TestTools AthenaKernel CxxUtils SGTools GaudiKernel TrigMuonEventTPCnv )

atlas_add_test( TrigMuonEFTrackCnv_p6_test
                SOURCES
                test/TrigMuonEFTrackCnv_p6_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} AthLinks DataModelAthenaPoolLib AthenaPoolCnvSvcLib AthenaPoolUtilities EventCommonTPCnv FourMom Particle TrigInDetEventTPCnv TrigMuonEvent TestTools AthenaKernel CxxUtils SGTools GaudiKernel TrigMuonEventTPCnv )

