################################################################################
# Package: TrigCaloEventTPCnv
################################################################################

# Declare the package name:
atlas_subdir( TrigCaloEventTPCnv )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthLinks
                          Control/DataModelAthenaPool
                          Database/AthenaPOOL/AthenaPoolCnvSvc
                          Database/AthenaPOOL/AthenaPoolUtilities
                          Trigger/TrigEvent/TrigCaloEvent
                          PRIVATE
                          AtlasTest/TestTools
                          Control/AthenaKernel
                          Control/CxxUtils
                          Control/SGTools
                          GaudiKernel )

# External dependencies:
find_package( CLHEP )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_tpcnv_library( TrigCaloEventTPCnv
                         src/*.cxx
                         PUBLIC_HEADERS TrigCaloEventTPCnv
                         PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                         PRIVATE_DEFINITIONS ${CLHEP_DEFINITIONS}
                         LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} AthLinks DataModelAthenaPoolLib AthenaPoolCnvSvcLib AthenaPoolUtilities TrigCaloEvent TestTools AthenaKernel CxxUtils SGTools GaudiKernel )

atlas_add_dictionary( TrigCaloEventTPCnvDict
                      TrigCaloEventTPCnv/TrigCaloEventTPCnvDict.h
                      TrigCaloEventTPCnv/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} AthLinks DataModelAthenaPoolLib AthenaPoolCnvSvcLib AthenaPoolUtilities TrigCaloEvent TestTools AthenaKernel CxxUtils SGTools GaudiKernel TrigCaloEventTPCnv )

atlas_add_test( TrigCaloClusterCnv_p1_test
                SOURCES
                test/TrigCaloClusterCnv_p1_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} AthLinks DataModelAthenaPoolLib AthenaPoolCnvSvcLib AthenaPoolUtilities TrigCaloEvent TestTools AthenaKernel CxxUtils SGTools GaudiKernel TrigCaloEventTPCnv )

atlas_add_test( TrigCaloClusterCnv_p2_test
                SOURCES
                test/TrigCaloClusterCnv_p2_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} AthLinks DataModelAthenaPoolLib AthenaPoolCnvSvcLib AthenaPoolUtilities TrigCaloEvent TestTools AthenaKernel CxxUtils SGTools GaudiKernel TrigCaloEventTPCnv )

atlas_add_test( TrigCaloClusterCnv_p3_test
                SOURCES
                test/TrigCaloClusterCnv_p3_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} AthLinks DataModelAthenaPoolLib AthenaPoolCnvSvcLib AthenaPoolUtilities TrigCaloEvent TestTools AthenaKernel CxxUtils SGTools GaudiKernel TrigCaloEventTPCnv )

atlas_add_test( RingerRingsCnv_p1_test
                SOURCES
                test/RingerRingsCnv_p1_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} AthLinks DataModelAthenaPoolLib AthenaPoolCnvSvcLib AthenaPoolUtilities TrigCaloEvent TestTools AthenaKernel CxxUtils SGTools GaudiKernel TrigCaloEventTPCnv )

atlas_add_test( RingerRingsCnv_p2_test
                SOURCES
                test/RingerRingsCnv_p2_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} AthLinks DataModelAthenaPoolLib AthenaPoolCnvSvcLib AthenaPoolUtilities TrigCaloEvent TestTools AthenaKernel CxxUtils SGTools GaudiKernel TrigCaloEventTPCnv )

atlas_add_test( TrigEMClusterCnv_p2_test
                SOURCES
                test/TrigEMClusterCnv_p2_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} AthLinks DataModelAthenaPoolLib AthenaPoolCnvSvcLib AthenaPoolUtilities TrigCaloEvent TestTools AthenaKernel CxxUtils SGTools GaudiKernel TrigCaloEventTPCnv )

atlas_add_test( TrigEMClusterCnv_p3_test
                SOURCES
                test/TrigEMClusterCnv_p3_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} AthLinks DataModelAthenaPoolLib AthenaPoolCnvSvcLib AthenaPoolUtilities TrigCaloEvent TestTools AthenaKernel CxxUtils SGTools GaudiKernel TrigCaloEventTPCnv )

atlas_add_test( TrigEMClusterCnv_p4_test
                SOURCES
                test/TrigEMClusterCnv_p4_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} AthLinks DataModelAthenaPoolLib AthenaPoolCnvSvcLib AthenaPoolUtilities TrigCaloEvent TestTools AthenaKernel CxxUtils SGTools GaudiKernel TrigCaloEventTPCnv )

atlas_add_test( TrigRNNOutputCnv_p1_test
                SOURCES
                test/TrigRNNOutputCnv_p1_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} AthLinks DataModelAthenaPoolLib AthenaPoolCnvSvcLib AthenaPoolUtilities TrigCaloEvent TestTools AthenaKernel CxxUtils SGTools GaudiKernel TrigCaloEventTPCnv )

atlas_add_test( TrigRNNOutputCnv_p2_test
                SOURCES
                test/TrigRNNOutputCnv_p2_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} AthLinks DataModelAthenaPoolLib AthenaPoolCnvSvcLib AthenaPoolUtilities TrigCaloEvent TestTools AthenaKernel CxxUtils SGTools GaudiKernel TrigCaloEventTPCnv )

atlas_add_test( TrigT2JetCnv_p2_test
                SOURCES
                test/TrigT2JetCnv_p2_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} AthLinks DataModelAthenaPoolLib AthenaPoolCnvSvcLib AthenaPoolUtilities TrigCaloEvent TestTools AthenaKernel CxxUtils SGTools GaudiKernel TrigCaloEventTPCnv )

atlas_add_test( TrigT2JetCnv_p3_test
                SOURCES
                test/TrigT2JetCnv_p3_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} AthLinks DataModelAthenaPoolLib AthenaPoolCnvSvcLib AthenaPoolUtilities TrigCaloEvent TestTools AthenaKernel CxxUtils SGTools GaudiKernel TrigCaloEventTPCnv )

atlas_add_test( TrigT2MbtsBitsCnv_p1_test
                SOURCES
                test/TrigT2MbtsBitsCnv_p1_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} AthLinks DataModelAthenaPoolLib AthenaPoolCnvSvcLib AthenaPoolUtilities TrigCaloEvent TestTools AthenaKernel CxxUtils SGTools GaudiKernel TrigCaloEventTPCnv )

atlas_add_test( TrigT2MbtsBitsCnv_p2_test
                SOURCES
                test/TrigT2MbtsBitsCnv_p2_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} AthLinks DataModelAthenaPoolLib AthenaPoolCnvSvcLib AthenaPoolUtilities TrigCaloEvent TestTools AthenaKernel CxxUtils SGTools GaudiKernel TrigCaloEventTPCnv )

atlas_add_test( TrigT2MbtsBitsCnv_p3_test
                SOURCES
                test/TrigT2MbtsBitsCnv_p3_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} AthLinks DataModelAthenaPoolLib AthenaPoolCnvSvcLib AthenaPoolUtilities TrigCaloEvent TestTools AthenaKernel CxxUtils SGTools GaudiKernel TrigCaloEventTPCnv )

atlas_add_test( TrigT2ZdcSignalsCnv_p1_test
                SOURCES
                test/TrigT2ZdcSignalsCnv_p1_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} AthLinks DataModelAthenaPoolLib AthenaPoolCnvSvcLib AthenaPoolUtilities TrigCaloEvent TestTools AthenaKernel CxxUtils SGTools GaudiKernel TrigCaloEventTPCnv )

atlas_add_test( TrigTauClusterCnv_p2_test
                SOURCES
                test/TrigTauClusterCnv_p2_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} AthLinks DataModelAthenaPoolLib AthenaPoolCnvSvcLib AthenaPoolUtilities TrigCaloEvent TestTools AthenaKernel CxxUtils SGTools GaudiKernel TrigCaloEventTPCnv )

atlas_add_test( TrigTauClusterCnv_p3_test
                SOURCES
                test/TrigTauClusterCnv_p3_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} AthLinks DataModelAthenaPoolLib AthenaPoolCnvSvcLib AthenaPoolUtilities TrigCaloEvent TestTools AthenaKernel CxxUtils SGTools GaudiKernel TrigCaloEventTPCnv )

atlas_add_test( TrigTauClusterCnv_p4_test
                SOURCES
                test/TrigTauClusterCnv_p4_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} AthLinks DataModelAthenaPoolLib AthenaPoolCnvSvcLib AthenaPoolUtilities TrigCaloEvent TestTools AthenaKernel CxxUtils SGTools GaudiKernel TrigCaloEventTPCnv )

atlas_add_test( TrigTauClusterCnv_p5_test
                SOURCES
                test/TrigTauClusterCnv_p5_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} AthLinks DataModelAthenaPoolLib AthenaPoolCnvSvcLib AthenaPoolUtilities TrigCaloEvent TestTools AthenaKernel CxxUtils SGTools GaudiKernel TrigCaloEventTPCnv )

atlas_add_test( TrigTauClusterDetailsCnv_p1_test
                SOURCES
                test/TrigTauClusterDetailsCnv_p1_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} AthLinks DataModelAthenaPoolLib AthenaPoolCnvSvcLib AthenaPoolUtilities TrigCaloEvent TestTools AthenaKernel CxxUtils SGTools GaudiKernel TrigCaloEventTPCnv )

atlas_add_test( TrigTauClusterDetailsCnv_p2_test
                SOURCES
                test/TrigTauClusterDetailsCnv_p2_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} AthLinks DataModelAthenaPoolLib AthenaPoolCnvSvcLib AthenaPoolUtilities TrigCaloEvent TestTools AthenaKernel CxxUtils SGTools GaudiKernel TrigCaloEventTPCnv )

