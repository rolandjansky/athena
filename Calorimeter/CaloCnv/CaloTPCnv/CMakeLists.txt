################################################################################
# Package: CaloTPCnv
################################################################################

# Declare the package name:
atlas_subdir( CaloTPCnv )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Calorimeter/CaloEvent
                          Control/AthenaKernel
                          Control/DataModel
                          Control/DataModelAthenaPool
                          Control/Navigation
                          Database/AthenaPOOL/AthenaPoolCnvSvc
                          Event/EventCommonTPCnv
                          GaudiKernel
                          PRIVATE
                          AtlasTest/TestTools
                          Calorimeter/CaloConditions
                          Calorimeter/CaloGeoHelpers
                          Calorimeter/CaloInterface
                          Calorimeter/CaloUtils
                          Control/CxxUtils
                          Control/SGTools
                          Event/EventKernel )

# External dependencies:
find_package( CLHEP )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_tpcnv_library( CaloTPCnv
                         src/*.cxx
                         PUBLIC_HEADERS CaloTPCnv
                         PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                         PRIVATE_DEFINITIONS ${CLHEP_DEFINITIONS}
                         LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} CaloEvent AthenaKernel DataModel DataModelAthenaPoolLib Navigation AthenaPoolCnvSvcLib EventCommonTPCnv GaudiKernel TestTools CaloConditions CaloGeoHelpers CaloUtilsLib SGTools EventKernel )

atlas_add_dictionary( CaloTPCnvDict
                      CaloTPCnv/CaloTPCnvDict.h
                      CaloTPCnv/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} CaloEvent AthenaKernel DataModel DataModelAthenaPoolLib Navigation AthenaPoolCnvSvcLib EventCommonTPCnv GaudiKernel TestTools CaloConditions CaloGeoHelpers CaloUtilsLib SGTools EventKernel CaloTPCnv )

atlas_add_dictionary( OLD_CaloTPCnvDict
                      CaloTPCnv/CaloTPCnvDict.h
                      CaloTPCnv/OLD_selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} CaloEvent AthenaKernel DataModel DataModelAthenaPoolLib Navigation AthenaPoolCnvSvcLib EventCommonTPCnv GaudiKernel TestTools CaloConditions CaloGeoHelpers CaloUtilsLib SGTools EventKernel CaloTPCnv )

atlas_add_test( CaloCellLinkContainerCnv_p2_test
                SOURCES
                test/CaloCellLinkContainerCnv_p2_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} CaloEvent AthenaKernel DataModel DataModelAthenaPoolLib Navigation AthenaPoolCnvSvcLib EventCommonTPCnv GaudiKernel TestTools CaloConditions CaloGeoHelpers CaloUtilsLib SGTools EventKernel CaloTPCnv
                EXTRA_PATTERNS "Athena::getMessageSvc" )

atlas_add_test( CaloClusterContainerCnv_test
                SOURCES
                test/CaloClusterContainerCnv_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} CaloEvent AthenaKernel DataModel DataModelAthenaPoolLib Navigation AthenaPoolCnvSvcLib EventCommonTPCnv GaudiKernel TestTools CaloConditions CaloGeoHelpers CaloUtilsLib SGTools EventKernel CaloTPCnv )

