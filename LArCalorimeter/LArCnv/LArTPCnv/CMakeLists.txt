################################################################################
# Package: LArTPCnv
################################################################################

# Declare the package name:
atlas_subdir( LArTPCnv )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Database/AthenaPOOL/AthenaPoolCnvSvc
                          LArCalorimeter/LArRawEvent
                          LArCalorimeter/LArRecEvent
                          Control/CxxUtils
                          PRIVATE
                          AtlasTest/TestTools
                          Calorimeter/CaloIdentifier
                          Control/AthenaKernel
                          DetectorDescription/Identifier )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_tpcnv_library( LArTPCnv
                         src/*.cxx
                         PUBLIC_HEADERS LArTPCnv
                         INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                         LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaPoolCnvSvcLib LArRawEvent LArRecEvent TestTools CaloIdentifier AthenaKernel Identifier )

atlas_add_dictionary( LArTPCnvDict
                      LArTPCnv/LArTPCnvDict.h
                      LArTPCnv/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaPoolCnvSvcLib LArRawEvent LArRecEvent TestTools CaloIdentifier AthenaKernel Identifier LArTPCnv )

atlas_add_dictionary( OLD_LArTPCnvDict
                      LArTPCnv/LArTPCnvDict.h
                      LArTPCnv/OLD_selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaPoolCnvSvcLib LArRawEvent LArRecEvent TestTools CaloIdentifier AthenaKernel Identifier LArTPCnv )

atlas_add_test( LArDigitContainerCnv_p1_test
                SOURCES
                test/LArDigitContainerCnv_p1_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaPoolCnvSvcLib LArRawEvent LArRecEvent TestTools CaloIdentifier AthenaKernel Identifier LArTPCnv )

atlas_add_test( LArFebErrorSummaryCnv_p1_test
                SOURCES
                test/LArFebErrorSummaryCnv_p1_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaPoolCnvSvcLib LArRawEvent LArRecEvent TestTools CaloIdentifier AthenaKernel Identifier LArTPCnv )

atlas_add_test( LArNoisyROSummaryCnv_p1_test
                SOURCES
                test/LArNoisyROSummaryCnv_p1_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaPoolCnvSvcLib LArRawEvent LArRecEvent TestTools CaloIdentifier AthenaKernel Identifier LArTPCnv )

atlas_add_test( LArNoisyROSummaryCnv_p2_test
                SOURCES
                test/LArNoisyROSummaryCnv_p2_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaPoolCnvSvcLib LArRawEvent LArRecEvent TestTools CaloIdentifier AthenaKernel Identifier LArTPCnv )

atlas_add_test( LArNoisyROSummaryCnv_p3_test
                SOURCES
                test/LArNoisyROSummaryCnv_p3_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaPoolCnvSvcLib LArRawEvent LArRecEvent TestTools CaloIdentifier AthenaKernel Identifier LArTPCnv )

atlas_add_test( LArNoisyROSummaryCnv_p4_test
                SOURCES
                test/LArNoisyROSummaryCnv_p4_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaPoolCnvSvcLib LArRawEvent LArRecEvent TestTools CaloIdentifier AthenaKernel Identifier LArTPCnv )

atlas_add_test( LArNoisyROSummaryCnv_p5_test
                SOURCES
                test/LArNoisyROSummaryCnv_p5_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaPoolCnvSvcLib LArRawEvent LArRecEvent TestTools CaloIdentifier AthenaKernel Identifier LArTPCnv )

atlas_add_test( LArRawChannelCnv_p2_test
                SOURCES
                test/LArRawChannelCnv_p2_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaPoolCnvSvcLib LArRawEvent LArRecEvent TestTools CaloIdentifier AthenaKernel Identifier LArTPCnv )

atlas_add_test( LArRawChannelContainerCnv_p4_test
                SOURCES
                test/LArRawChannelContainerCnv_p4_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaPoolCnvSvcLib LArRawEvent LArRecEvent TestTools CaloIdentifier AthenaKernel Identifier LArTPCnv )

atlas_add_test( LArLATOMEHeaderContainerCnv_p1_test
                SOURCES
                test/LArLATOMEHeaderContainerCnv_p1_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaPoolCnvSvcLib LArRawEvent LArRecEvent TestTools CaloIdentifier AthenaKernel Identifier LArTPCnv )

