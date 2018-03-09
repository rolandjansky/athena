################################################################################
# Package: LArSimEventTPCnv
################################################################################

# Declare the package name:
atlas_subdir( LArSimEventTPCnv )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Database/AthenaPOOL/AthenaPoolCnvSvc
                          LArCalorimeter/LArSimEvent
                          PRIVATE
                          AtlasTest/TestTools
                          Calorimeter/CaloIdentifier
                          Control/AthenaKernel
                          Control/StoreGate
                          DetectorDescription/IdDictParser
                          DetectorDescription/Identifier )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_tpcnv_library( LArSimEventTPCnv
                         src/*.cxx
                         PUBLIC_HEADERS LArSimEventTPCnv
                         INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                         LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaPoolCnvSvcLib LArSimEvent TestTools CaloIdentifier AthenaKernel StoreGateLib SGtests IdDictParser Identifier )

atlas_add_dictionary( LArSimEventTPCnvDict
                      LArSimEventTPCnv/LArSimEventTPCnvDict.h
                      LArSimEventTPCnv/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaPoolCnvSvcLib LArSimEvent TestTools CaloIdentifier AthenaKernel StoreGateLib SGtests IdDictParser Identifier LArSimEventTPCnv )

atlas_add_dictionary( OLD_LArSimEventTPCnvDict
                      LArSimEventTPCnv/LArSimEventTPCnvDict.h
                      LArSimEventTPCnv/OLD_selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaPoolCnvSvcLib LArSimEvent TestTools CaloIdentifier AthenaKernel StoreGateLib SGtests IdDictParser Identifier LArSimEventTPCnv )

atlas_add_test( LArHitCnv_p1_test
                SOURCES
                test/LArHitCnv_p1_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaPoolCnvSvcLib LArSimEvent TestTools CaloIdentifier AthenaKernel StoreGateLib SGtests IdDictParser Identifier LArSimEventTPCnv )

atlas_add_test( LArHitContainerCnv_p1_test
                SOURCES
                test/LArHitContainerCnv_p1_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaPoolCnvSvcLib LArSimEvent TestTools CaloIdentifier AthenaKernel StoreGateLib SGtests IdDictParser Identifier LArSimEventTPCnv )

atlas_add_test( LArHitContainerCnv_p2_test
                SOURCES
                test/LArHitContainerCnv_p2_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaPoolCnvSvcLib LArSimEvent TestTools CaloIdentifier AthenaKernel StoreGateLib SGtests IdDictParser Identifier LArSimEventTPCnv
                ENVIRONMENT "JOBOPTSEARCHPATH=${CMAKE_CURRENT_SOURCE_DIR}/share" )

atlas_add_test( LArHitFloatCnv_p1_test
                SOURCES
                test/LArHitFloatCnv_p1_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaPoolCnvSvcLib LArSimEvent TestTools CaloIdentifier AthenaKernel StoreGateLib SGtests IdDictParser Identifier LArSimEventTPCnv )

atlas_add_test( LArHitFloatContainerCnv_p2_test
                SOURCES
                test/LArHitFloatContainerCnv_p2_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaPoolCnvSvcLib LArSimEvent TestTools CaloIdentifier AthenaKernel StoreGateLib SGtests IdDictParser Identifier LArSimEventTPCnv 
                ENVIRONMENT "JOBOPTSEARCHPATH=${CMAKE_CURRENT_SOURCE_DIR}/share" )

