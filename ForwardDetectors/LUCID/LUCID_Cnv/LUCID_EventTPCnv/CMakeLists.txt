################################################################################
# Package: LUCID_EventTPCnv
################################################################################

# Declare the package name:
atlas_subdir( LUCID_EventTPCnv )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Database/AthenaPOOL/AthenaPoolCnvSvc
                          Database/AthenaPOOL/AthenaPoolUtilities
                          ForwardDetectors/LUCID/LUCID_RawEvent
                          ForwardDetectors/LUCID/LUCID_SimUtils/LUCID_SimEvent
                          Generators/GeneratorObjectsTPCnv
                          PRIVATE
                          AtlasTest/TestTools
                          DetectorDescription/Identifier
                          GaudiKernel )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( LUCID_EventTPCnv
                   src/*.cxx
                   PUBLIC_HEADERS LUCID_EventTPCnv
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES AthenaPoolUtilities LUCID_RawEvent LUCID_SimEvent GeneratorObjectsTPCnv AthenaPoolCnvSvcLib
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} TestTools Identifier GaudiKernel )

atlas_add_dictionary( LUCID_EventTPCnvDict
                      LUCID_EventTPCnv/LUCID_EventTPCnvDict.h
                      LUCID_EventTPCnv/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaPoolCnvSvcLib AthenaPoolUtilities LUCID_RawEvent LUCID_SimEvent GeneratorObjectsTPCnv TestTools Identifier GaudiKernel LUCID_EventTPCnv )

atlas_add_test( LUCID_DigitCnv_p1_test
                SOURCES
                test/LUCID_DigitCnv_p1_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaPoolCnvSvcLib AthenaPoolUtilities LUCID_RawEvent LUCID_SimEvent GeneratorObjectsTPCnv TestTools Identifier GaudiKernel LUCID_EventTPCnv )

atlas_add_test( LUCID_DigitCnv_p2_test
                SOURCES
                test/LUCID_DigitCnv_p2_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaPoolCnvSvcLib AthenaPoolUtilities LUCID_RawEvent LUCID_SimEvent GeneratorObjectsTPCnv TestTools Identifier GaudiKernel LUCID_EventTPCnv )

atlas_add_test( LUCID_DigitContainerCnv_p1_test
                SOURCES
                test/LUCID_DigitContainerCnv_p1_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaPoolCnvSvcLib AthenaPoolUtilities LUCID_RawEvent LUCID_SimEvent GeneratorObjectsTPCnv TestTools Identifier GaudiKernel LUCID_EventTPCnv )

atlas_add_test( LUCID_DigitContainerCnv_p2_test
                SOURCES
                test/LUCID_DigitContainerCnv_p2_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaPoolCnvSvcLib AthenaPoolUtilities LUCID_RawEvent LUCID_SimEvent GeneratorObjectsTPCnv TestTools Identifier GaudiKernel LUCID_EventTPCnv )

atlas_add_test( LUCID_RawDataCnv_p1_test
                SOURCES
                test/LUCID_RawDataCnv_p1_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaPoolCnvSvcLib AthenaPoolUtilities LUCID_RawEvent LUCID_SimEvent GeneratorObjectsTPCnv TestTools Identifier GaudiKernel LUCID_EventTPCnv )

atlas_add_test( LUCID_RawDataContainerCnv_p1_test
                SOURCES
                test/LUCID_RawDataContainerCnv_p1_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaPoolCnvSvcLib AthenaPoolUtilities LUCID_RawEvent LUCID_SimEvent GeneratorObjectsTPCnv TestTools Identifier GaudiKernel LUCID_EventTPCnv )

atlas_add_test( LUCID_SimHitCnv_p1_test
                SOURCES
                test/LUCID_SimHitCnv_p1_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaPoolCnvSvcLib AthenaPoolUtilities LUCID_RawEvent LUCID_SimEvent GeneratorObjectsTPCnv TestTools Identifier GaudiKernel LUCID_EventTPCnv )

atlas_add_test( LUCID_SimHitCnv_p2_test
                SOURCES
                test/LUCID_SimHitCnv_p2_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaPoolCnvSvcLib AthenaPoolUtilities LUCID_RawEvent LUCID_SimEvent GeneratorObjectsTPCnv TestTools Identifier GaudiKernel LUCID_EventTPCnv )

