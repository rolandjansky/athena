################################################################################
# Package: CaloTPCnv
################################################################################

# Declare the package name:
atlas_subdir( CaloTPCnv )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Calorimeter/CaloEvent
                          Control/AthenaKernel
                          Control/AthAllocators
                          Control/AthLinks
                          Control/DataModelAthenaPool
                          Control/Navigation
                          Database/AthenaPOOL/AthenaPoolCnvSvc
                          Event/EventCommonTPCnv
                          GaudiKernel
                          PRIVATE
                          AtlasTest/TestTools
                          Calorimeter/CaloConditions
                          Calorimeter/CaloGeoHelpers
                          #Calorimeter/CaloInterface
                          Calorimeter/CaloUtils
                          Control/CxxUtils
                          Control/SGTools
                          Event/EventKernel
			  LArCalorimeter/LArRecEvent 
			  TileCalorimeter/TileEvent
                          DetectorDescription/IdDictParser
                          )

# External dependencies:
find_package( CLHEP )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_tpcnv_library( CaloTPCnv
                         src/*.cxx
                         PUBLIC_HEADERS CaloTPCnv
                         PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                         PRIVATE_DEFINITIONS ${CLHEP_DEFINITIONS}
                         LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} CaloEvent AthenaKernel AthAllocators AthLinks DataModelAthenaPoolLib Navigation AthenaPoolCnvSvcLib EventCommonTPCnv GaudiKernel TestTools CaloConditions CaloGeoHelpers CaloUtilsLib SGTools EventKernel LArRecEvent TileEvent )

atlas_add_dictionary( CaloTPCnvDict
                      CaloTPCnv/CaloTPCnvDict.h
                      CaloTPCnv/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} CaloEvent AthenaKernel AthAllocators AthLinks DataModelAthenaPoolLib Navigation AthenaPoolCnvSvcLib EventCommonTPCnv GaudiKernel TestTools CaloConditions CaloGeoHelpers CaloUtilsLib SGTools EventKernel CaloTPCnv  LArRecEvent TileEvent)

atlas_add_dictionary( OLD_CaloTPCnvDict
                      CaloTPCnv/CaloTPCnvDict.h
                      CaloTPCnv/OLD_selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} CaloEvent AthenaKernel AthAllocators AthLinks DataModelAthenaPoolLib Navigation AthenaPoolCnvSvcLib EventCommonTPCnv GaudiKernel TestTools CaloConditions CaloGeoHelpers CaloUtilsLib SGTools EventKernel CaloTPCnv  LArRecEvent TileEvent)

atlas_add_test( CaloCellLinkContainerCnv_p2_test
                SOURCES
                test/CaloCellLinkContainerCnv_p2_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} CaloEvent AthenaKernel AthAllocators AthLinks DataModelAthenaPoolLib Navigation AthenaPoolCnvSvcLib EventCommonTPCnv GaudiKernel TestTools CaloConditions CaloGeoHelpers CaloUtilsLib SGTools EventKernel CaloTPCnv
                EXTRA_PATTERNS "Athena::getMessageSvc" )

atlas_add_test( CaloClusterContainerCnv_test
                SOURCES
                test/CaloClusterContainerCnv_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} CaloEvent AthenaKernel AthAllocators AthLinks DataModelAthenaPoolLib Navigation AthenaPoolCnvSvcLib EventCommonTPCnv GaudiKernel TestTools CaloConditions CaloGeoHelpers CaloUtilsLib SGTools EventKernel CaloTPCnv )


atlas_add_test( CaloClusterCellLinkContainerCnv_p1_test
                SOURCES
                test/CaloClusterCellLinkContainerCnv_p1_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} CaloEvent AthenaKernel AthAllocators AthLinks DataModelAthenaPoolLib Navigation AthenaPoolCnvSvcLib EventCommonTPCnv GaudiKernel TestTools CaloConditions CaloGeoHelpers CaloUtilsLib SGTools EventKernel CaloTPCnv )


atlas_add_test( CaloCellPackerUtils_test
   SOURCES test/CaloCellPackerUtils_test.cxx
   LINK_LIBRARIES GaudiKernel )

atlas_add_test( CaloCompactCellTool_test
   SOURCES test/CaloCompactCellTool_test.cxx
   LINK_LIBRARIES CaloDetDescrLib CaloEvent CaloIdentifier
   AthenaKernel StoreGateLib GaudiKernel LArRecEvent TileEvent TestTools
   IdDictParser CaloTPCnv
   EXTRA_PATTERNS "SGAudSvc +INFO|^lar decode|^HistogramPersis.* INFO|initialize_from_dict|mask/zero|^JobOptionsSvc +INFO|^AtlasDetectorID::"
   ENVIRONMENT "JOBOPTSEARCHPATH=${CMAKE_CURRENT_SOURCE_DIR}/share" )

