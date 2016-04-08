################################################################################
# Package: TrigT1EventTPCnv
################################################################################

# Declare the package name:
atlas_subdir( TrigT1EventTPCnv )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Database/AthenaPOOL/AthenaPoolCnvSvc
                          Database/AthenaPOOL/AthenaPoolUtilities
                          Trigger/TrigT1/TrigT1CaloEvent
                          Trigger/TrigT1/TrigT1Result
                          PRIVATE
                          AtlasTest/TestTools
                          Control/AthenaKernel
                          GaudiKernel )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_tpcnv_library( TrigT1EventTPCnv
                         src/*.cxx
                         PUBLIC_HEADERS TrigT1EventTPCnv
                         PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                         LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaPoolCnvSvcLib AthenaPoolUtilities TrigT1CaloEventLib TrigT1Result TestTools AthenaKernel GaudiKernel )

atlas_add_dictionary( TrigT1EventTPCnvDict
                      TrigT1EventTPCnv/TrigT1EventTPCnvDict.h
                      TrigT1EventTPCnv/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaPoolCnvSvcLib AthenaPoolUtilities TrigT1CaloEventLib TrigT1Result TestTools AthenaKernel GaudiKernel TrigT1EventTPCnv )

atlas_add_test( CMMCPHitsCnv_p1_test
                SOURCES
                test/CMMCPHitsCnv_p1_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaPoolCnvSvcLib AthenaPoolUtilities TrigT1CaloEventLib TrigT1Result TestTools AthenaKernel GaudiKernel TrigT1EventTPCnv )

atlas_add_test( CMMEtSumsCnv_p1_test
                SOURCES
                test/CMMEtSumsCnv_p1_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaPoolCnvSvcLib AthenaPoolUtilities TrigT1CaloEventLib TrigT1Result TestTools AthenaKernel GaudiKernel TrigT1EventTPCnv )

atlas_add_test( CMMJetHitsCnv_p1_test
                SOURCES
                test/CMMJetHitsCnv_p1_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaPoolCnvSvcLib AthenaPoolUtilities TrigT1CaloEventLib TrigT1Result TestTools AthenaKernel GaudiKernel TrigT1EventTPCnv )

atlas_add_test( CMMRoICnv_p1_test
                SOURCES
                test/CMMRoICnv_p1_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaPoolCnvSvcLib AthenaPoolUtilities TrigT1CaloEventLib TrigT1Result TestTools AthenaKernel GaudiKernel TrigT1EventTPCnv )

atlas_add_test( CMXCPHitsCnv_p1_test
                SOURCES
                test/CMXCPHitsCnv_p1_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaPoolCnvSvcLib AthenaPoolUtilities TrigT1CaloEventLib TrigT1Result TestTools AthenaKernel GaudiKernel TrigT1EventTPCnv )

atlas_add_test( CMXCPTobCnv_p1_test
                SOURCES
                test/CMXCPTobCnv_p1_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaPoolCnvSvcLib AthenaPoolUtilities TrigT1CaloEventLib TrigT1Result TestTools AthenaKernel GaudiKernel TrigT1EventTPCnv )

atlas_add_test( CMXEtSumsCnv_p1_test
                SOURCES
                test/CMXEtSumsCnv_p1_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaPoolCnvSvcLib AthenaPoolUtilities TrigT1CaloEventLib TrigT1Result TestTools AthenaKernel GaudiKernel TrigT1EventTPCnv )

atlas_add_test( CMXJetHitsCnv_p1_test
                SOURCES
                test/CMXJetHitsCnv_p1_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaPoolCnvSvcLib AthenaPoolUtilities TrigT1CaloEventLib TrigT1Result TestTools AthenaKernel GaudiKernel TrigT1EventTPCnv )

atlas_add_test( CMXJetTobCnv_p1_test
                SOURCES
                test/CMXJetTobCnv_p1_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaPoolCnvSvcLib AthenaPoolUtilities TrigT1CaloEventLib TrigT1Result TestTools AthenaKernel GaudiKernel TrigT1EventTPCnv )

atlas_add_test( CMXRoICnv_p1_test
                SOURCES
                test/CMXRoICnv_p1_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaPoolCnvSvcLib AthenaPoolUtilities TrigT1CaloEventLib TrigT1Result TestTools AthenaKernel GaudiKernel TrigT1EventTPCnv )

atlas_add_test( CPMHitsCnv_p1_test
                SOURCES
                test/CPMHitsCnv_p1_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaPoolCnvSvcLib AthenaPoolUtilities TrigT1CaloEventLib TrigT1Result TestTools AthenaKernel GaudiKernel TrigT1EventTPCnv )

atlas_add_test( CPMRoICnv_p1_test
                SOURCES
                test/CPMRoICnv_p1_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaPoolCnvSvcLib AthenaPoolUtilities TrigT1CaloEventLib TrigT1Result TestTools AthenaKernel GaudiKernel TrigT1EventTPCnv )

atlas_add_test( CPMTobRoICnv_p1_test
                SOURCES
                test/CPMTobRoICnv_p1_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaPoolCnvSvcLib AthenaPoolUtilities TrigT1CaloEventLib TrigT1Result TestTools AthenaKernel GaudiKernel TrigT1EventTPCnv )

atlas_add_test( CPMTowerCnv_p1_test
                SOURCES
                test/CPMTowerCnv_p1_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaPoolCnvSvcLib AthenaPoolUtilities TrigT1CaloEventLib TrigT1Result TestTools AthenaKernel GaudiKernel TrigT1EventTPCnv )

atlas_add_test( CTP_RDOCnv_p1_test
                SOURCES
                test/CTP_RDOCnv_p1_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaPoolCnvSvcLib AthenaPoolUtilities TrigT1CaloEventLib TrigT1Result TestTools AthenaKernel GaudiKernel TrigT1EventTPCnv )

atlas_add_test( CTP_RDOCnv_p2_test
                SOURCES
                test/CTP_RDOCnv_p2_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaPoolCnvSvcLib AthenaPoolUtilities TrigT1CaloEventLib TrigT1Result TestTools AthenaKernel GaudiKernel TrigT1EventTPCnv )

atlas_add_test( JEMEtSumsCnv_p1_test
                SOURCES
                test/JEMEtSumsCnv_p1_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaPoolCnvSvcLib AthenaPoolUtilities TrigT1CaloEventLib TrigT1Result TestTools AthenaKernel GaudiKernel TrigT1EventTPCnv )

atlas_add_test( JEMHitsCnv_p1_test
                SOURCES
                test/JEMHitsCnv_p1_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaPoolCnvSvcLib AthenaPoolUtilities TrigT1CaloEventLib TrigT1Result TestTools AthenaKernel GaudiKernel TrigT1EventTPCnv )

atlas_add_test( JEMRoICnv_p1_test
                SOURCES
                test/JEMRoICnv_p1_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaPoolCnvSvcLib AthenaPoolUtilities TrigT1CaloEventLib TrigT1Result TestTools AthenaKernel GaudiKernel TrigT1EventTPCnv )

atlas_add_test( JEMTobRoICnv_p1_test
                SOURCES
                test/JEMTobRoICnv_p1_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaPoolCnvSvcLib AthenaPoolUtilities TrigT1CaloEventLib TrigT1Result TestTools AthenaKernel GaudiKernel TrigT1EventTPCnv )

atlas_add_test( JetElementCnv_p1_test
                SOURCES
                test/JetElementCnv_p1_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaPoolCnvSvcLib AthenaPoolUtilities TrigT1CaloEventLib TrigT1Result TestTools AthenaKernel GaudiKernel TrigT1EventTPCnv )

atlas_add_test( MuCTPI_RDOCnv_p1_test
                SOURCES
                test/MuCTPI_RDOCnv_p1_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaPoolCnvSvcLib AthenaPoolUtilities TrigT1CaloEventLib TrigT1Result TestTools AthenaKernel GaudiKernel TrigT1EventTPCnv )

atlas_add_test( RODHeaderCnv_p1_test
                SOURCES
                test/RODHeaderCnv_p1_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaPoolCnvSvcLib AthenaPoolUtilities TrigT1CaloEventLib TrigT1Result TestTools AthenaKernel GaudiKernel TrigT1EventTPCnv )

atlas_add_test( RoIBResultCnv_p1_test
                SOURCES
                test/RoIBResultCnv_p1_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaPoolCnvSvcLib AthenaPoolUtilities TrigT1CaloEventLib TrigT1Result TestTools AthenaKernel GaudiKernel TrigT1EventTPCnv )

atlas_add_test( TriggerTowerCnv_p1_test
                SOURCES
                test/TriggerTowerCnv_p1_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaPoolCnvSvcLib AthenaPoolUtilities TrigT1CaloEventLib TrigT1Result TestTools AthenaKernel GaudiKernel TrigT1EventTPCnv )

