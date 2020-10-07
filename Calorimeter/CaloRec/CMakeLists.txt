################################################################################
# Package: CaloRec
################################################################################

# Declare the package name:
atlas_subdir( CaloRec )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   Calorimeter/CaloConditions
   Calorimeter/CaloEvent
   Control/AthenaBaseComps
   Control/AthenaKernel
   Control/StoreGate
   Database/AthenaPOOL/AthenaPoolUtilities
   Event/xAOD/xAODCaloEvent
   GaudiKernel
   PRIVATE
   AtlasTest/TestTools
   Control/CxxUtils
   Control/AthAllocators
   DetectorDescription/IdDictParser
   DetectorDescription/Identifier
   Event/EventKernel
   Event/FourMom
   Event/NavFourMom
   Event/xAOD/xAODEventInfo	
   Calorimeter/CaloDetDescr
   Calorimeter/CaloGeoHelpers
   Calorimeter/CaloIdentifier
   Calorimeter/CaloInterface
   Calorimeter/CaloUtils
   Calorimeter/CaloLumiConditions
   LArCalorimeter/LArTools
   LArCalorimeter/LArElecCalib
   LArCalorimeter/LArIdentifier
   LArCalorimeter/LArRawConditions
   LumiBlock/LumiBlockComps
   Control/AthenaMonitoringKernel )

# External dependencies:
find_package( AIDA )
find_package( CLHEP )
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )
find_package( Eigen )
find_package( ROOT COMPONENTS Core MathCore MathMore Minuit Minuit2 Matrix )

# Component(s) in the package:
atlas_add_library( CaloRecLib
   CaloRec/*.h CaloRec/*.icc src/*.cxx
   PUBLIC_HEADERS CaloRec
   INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
   PRIVATE_INCLUDE_DIRS ${AIDA_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
   ${CORAL_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
   DEFINITIONS ${CLHEP_DEFINITIONS}
   LINK_LIBRARIES ${CLHEP_LIBRARIES} CaloConditions CaloEvent CaloGeoHelpers
   CaloIdentifier AthenaBaseComps AthenaKernel CxxUtils AthenaPoolUtilities
   Identifier xAODCaloEvent GaudiKernel CaloDetDescrLib CaloUtilsLib
   StoreGateLib LArToolsLib LumiBlockCompsLib AthenaMonitoringKernelLib
   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} ${CORAL_LIBRARIES}
   ${EIGEN_LIBRARIES} AthAllocators IdDictParser EventKernel CaloLumiConditions
   LArRawConditions
   FourMom NavFourMom )

atlas_add_component( CaloRec
   src/components/*.cxx
   LINK_LIBRARIES CaloRecLib CaloLumiConditions )

# Test(s) in the package:
atlas_add_test( CaloClusterProcessor_test
   SOURCES test/CaloClusterProcessor_test.cxx
   LINK_LIBRARIES CaloRecLib xAODCaloEvent TestTools AthenaBaseComps )

atlas_add_test( CaloCellFastCopyTool_test
   SOURCES test/CaloCellFastCopyTool_test.cxx
   LINK_LIBRARIES CaloRecLib CaloEvent CaloDetDescrLib CaloIdentifier
   IdDictParser AthenaBaseComps StoreGateLib CxxUtils GaudiKernel
   LOG_IGNORE_PATTERN "Retrieved const pointer|Retrieved const handle|object not modifiable" )

atlas_add_test( CaloCellContainerFromClusterTool_test
   SOURCES test/CaloCellContainerFromClusterTool_test.cxx
   LINK_LIBRARIES CaloRecLib 
   ENVIRONMENT "JOBOPTSEARCHPATH=${CMAKE_CURRENT_SOURCE_DIR}/share" )

atlas_add_test( CaloBCIDCoeffsCondAlg_test
   SOURCES test/CaloBCIDCoeffsCondAlg_test.cxx
   LINK_LIBRARIES CaloRecLib CaloLumiConditions LArRawConditions IdDictParser TestTools )

atlas_add_test( CaloBCIDLumiCondAlg_test
   SOURCES test/CaloBCIDLumiCondAlg_test.cxx
   LINK_LIBRARIES CaloRecLib CaloLumiConditions LArRawConditions IdDictParser TestTools )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py share/*.txt )


atlas_add_test( CaloBCIDAvgAlgConfig_test
                SCRIPT python -m CaloRec.CaloBCIDAvgAlgConfig
                LOG_SELECT_PATTERN "ComponentAccumulator|^---|^IOVDbSvc" )


atlas_add_test( CaloBCIDCoeffsCondAlgConfig_test
                SCRIPT python -m CaloRec.CaloBCIDCoeffsCondAlgConfig
                LOG_SELECT_PATTERN "ComponentAccumulator|^---|^IOVDbSvc" )


atlas_add_test( CaloBCIDLumiCondAlgConfig_test
                SCRIPT python -m CaloRec.CaloBCIDLumiCondAlgConfig
                LOG_SELECT_PATTERN "ComponentAccumulator|^---|^IOVDbSvc" )


atlas_add_test( CaloTopoClusterConfig_test
                SCRIPT python -m CaloRec.CaloTopoClusterConfig
                PROPERTIES TIMEOUT 300
                POST_EXEC_SCRIPT nopost.sh)


atlas_add_test( CaloCellContainerAliasAlg_test
                SCRIPT python -m CaloRec.CaloCellContainerAliasAlg_test
                PROPERTIES TIMEOUT 300
                LOG_SELECT_PATTERN "ERROR|error|WARNING [^U]|FATAL|processing" )


atlas_add_test( CaloThinCellsBySamplingAlg_test
                SCRIPT python -m CaloRec.CaloThinCellsBySamplingAlg_test
                PROPERTIES TIMEOUT 600
                LOG_SELECT_PATTERN "ERROR|error|WARNING [^U]|FATAL|processing" )


atlas_add_test( CaloThinCellsByClusterAlg_test
                SCRIPT python -m CaloRec.CaloThinCellsByClusterAlg_test
                PROPERTIES TIMEOUT 600
                LOG_SELECT_PATTERN "ERROR|error|WARNING [^U]|FATAL|processing" )


atlas_add_test( ToolConstantsCondAlg_test
                SCRIPT test/ToolConstantsCondAlg_test.py
                LOG_IGNORE_PATTERN "Cache alignment|Current filenames|Unable to locate catalog|peeking into|IOVDbSvc +INFO" )


atlas_add_test( CaloCellContainerAliasAlgConfig_test
                SCRIPT python -m CaloRec.CaloCellContainerAliasAlgConfig
                LOG_SELECT_PATTERN "ComponentAccumulator" )


atlas_add_test( CaloThinCellsByClusterAlgConfig_test
                SCRIPT python -m CaloRec.CaloThinCellsByClusterAlgConfig
                LOG_SELECT_PATTERN "ComponentAccumulator" )


atlas_add_test( CaloThinCellsBySamplingAlgConfig_test
                SCRIPT python -m CaloRec.CaloThinCellsBySamplingAlgConfig
                LOG_SELECT_PATTERN "ComponentAccumulator" )

atlas_add_test( ToolConstantsCondAlgConfig_test
                SCRIPT python -m CaloRec.ToolConstantsCondAlgConfig
                LOG_SELECT_PATTERN "ComponentAccumulator|^---" )

