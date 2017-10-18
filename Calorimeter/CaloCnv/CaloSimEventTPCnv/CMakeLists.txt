################################################################################
# Package: CaloSimEventTPCnv
################################################################################

# Declare the package name:
atlas_subdir( CaloSimEventTPCnv )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Calorimeter/CaloSimEvent
                          Database/AthenaPOOL/AthenaPoolCnvSvc
                          DetectorDescription/Identifier
                          PRIVATE
                          AtlasTest/TestTools
                          Calorimeter/CaloIdentifier
                          Control/AthenaKernel
                          GaudiKernel )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_tpcnv_library( CaloSimEventTPCnv
                         src/*.cxx
                         PUBLIC_HEADERS CaloSimEventTPCnv
                         PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                         LINK_LIBRARIES ${ROOT_LIBRARIES} CaloSimEvent AthenaPoolCnvSvcLib Identifier TestTools CaloIdentifier AthenaKernel GaudiKernel )

atlas_add_dictionary( CaloSimEventTPCnvDict
                      CaloSimEventTPCnv/CaloSimEventTPCnvDict.h
                      CaloSimEventTPCnv/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} CaloSimEvent AthenaPoolCnvSvcLib Identifier TestTools CaloIdentifier AthenaKernel GaudiKernel CaloSimEventTPCnv )

atlas_add_dictionary( OLD_CaloSimEventTPCnvDict
                      CaloSimEventTPCnv/CaloSimEventTPCnvDict.h
                      CaloSimEventTPCnv/OLD_selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} CaloSimEvent AthenaPoolCnvSvcLib Identifier TestTools CaloIdentifier AthenaKernel GaudiKernel CaloSimEventTPCnv )

atlas_add_test( CaloCalibrationHitCnv_p1_test
                SOURCES
                test/CaloCalibrationHitCnv_p1_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} CaloSimEvent AthenaPoolCnvSvcLib Identifier TestTools CaloIdentifier AthenaKernel GaudiKernel CaloSimEventTPCnv )

atlas_add_test( CaloCalibrationHitCnv_p2_test
                SOURCES
                test/CaloCalibrationHitCnv_p2_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} CaloSimEvent AthenaPoolCnvSvcLib Identifier TestTools CaloIdentifier AthenaKernel GaudiKernel CaloSimEventTPCnv )

atlas_add_test( CaloCalibrationHitContainerCnv_p1_test
                SOURCES
                test/CaloCalibrationHitContainerCnv_p1_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} CaloSimEvent AthenaPoolCnvSvcLib Identifier TestTools CaloIdentifier AthenaKernel GaudiKernel CaloSimEventTPCnv
                EXTRA_PATTERNS "Athena::getMessageSvc" )

atlas_add_test( CaloCalibrationHitContainerCnv_p2_test
                SOURCES
                test/CaloCalibrationHitContainerCnv_p2_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} CaloSimEvent AthenaPoolCnvSvcLib Identifier TestTools CaloIdentifier AthenaKernel GaudiKernel CaloSimEventTPCnv
                EXTRA_PATTERNS "Athena::getMessageSvc" )

atlas_add_test( CaloCalibrationHitContainerCnv_p3_test
                SOURCES
                test/CaloCalibrationHitContainerCnv_p3_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} CaloSimEvent AthenaPoolCnvSvcLib Identifier TestTools CaloIdentifier AthenaKernel GaudiKernel CaloSimEventTPCnv
                EXTRA_PATTERNS "Athena::getMessageSvc" )

