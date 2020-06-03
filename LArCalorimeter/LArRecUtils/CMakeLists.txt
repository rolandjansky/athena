################################################################################
# Package: LArRecUtils
################################################################################

# Declare the package name:
atlas_subdir( LArRecUtils )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Calorimeter/CaloDetDescr
                          Calorimeter/CaloEvent
                          Calorimeter/CaloIdentifier
                          Calorimeter/CaloInterface
                          Calorimeter/CaloUtils
                          Calorimeter/CaloConditions
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Control/AthAllocators
                          Control/StoreGate
                          Database/AthenaPOOL/AthenaPoolUtilities
                          DetectorDescription/Identifier
                          GaudiKernel
                          LArCalorimeter/LArCabling
                          LArCalorimeter/LArElecCalib
                          LArCalorimeter/LArIdentifier
                          LArCalorimeter/LArRawConditions
                          LArCalorimeter/LArRawEvent
                          LArCalorimeter/LArRecEvent
                          PRIVATE
                          AtlasTest/TestTools
                          Calorimeter/CaloGeoHelpers
                          Control/SGTools
                          Event/xAOD/xAODEventInfo
                          LArCalorimeter/LArCOOLConditions
			  LArCalorimeter/LArRecConditions
                          LArCalorimeter/LArRawUtils
                          Tools/PathResolver )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( CLHEP )
find_package( Eigen )
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )

# Component(s) in the package:
atlas_add_library( LArRecUtilsLib
                   src/*.cxx
                   PUBLIC_HEADERS LArRecUtils
                   INCLUDE_DIRS ${CORAL_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                   PRIVATE_DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${CORAL_LIBRARIES} ${EIGEN_LIBRARIES} CaloEvent CaloIdentifier AthenaBaseComps AthenaKernel AthAllocators AthenaPoolUtilities Identifier GaudiKernel LArIdentifier LArRawEvent LArRecEvent CaloDetDescrLib CaloUtilsLib StoreGateLib SGtests LArCablingLib LArRawUtilsLib LArRecConditions LArRawConditions
                   PRIVATE_LINK_LIBRARIES ${Boost_LIBRARIES} ${CLHEP_LIBRARIES} ${EIGEN_LIBRARIES} TestTools CaloGeoHelpers SGTools xAODEventInfo LArCOOLConditions PathResolver CaloConditions )

atlas_add_component( LArRecUtils
                     src/components/*.cxx
                     INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                     LINK_LIBRARIES ${Boost_LIBRARIES} ${CORAL_LIBRARIES} ${CLHEP_LIBRARIES} ${EIGEN_LIBRARIES} CaloDetDescrLib CaloEvent CaloIdentifier CaloUtilsLib AthenaBaseComps AthenaKernel AthAllocators StoreGateLib SGtests AthenaPoolUtilities Identifier GaudiKernel LArCablingLib LArIdentifier LArRawConditions LArRawEvent LArRecEvent TestTools CaloGeoHelpers SGTools xAODEventInfo LArCOOLConditions LArRawUtilsLib PathResolver LArRecUtilsLib LArRecConditions LArRawConditions CaloConditions)

atlas_add_test( dummy_test
                SOURCES
                test/dummy_test.cxx
                INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                LINK_LIBRARIES ${Boost_LIBRARIES} ${CORAL_LIBRARIES} ${CLHEP_LIBRARIES} ${EIGEN_LIBRARIES} CaloDetDescrLib CaloEvent CaloIdentifier CaloUtilsLib AthenaBaseComps AthenaKernel AthAllocators StoreGateLib SGtests AthenaPoolUtilities Identifier GaudiKernel LArCablingLib LArIdentifier LArRawConditions LArRawEvent LArRecEvent TestTools CaloGeoHelpers SGTools xAODEventInfo LArCOOLConditions LArRawUtilsLib CaloConditions PathResolver LArRecUtilsLib )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} --extend-ignore=F401,F821 )

atlas_add_test( LArFCalTowerBuilderTool
                SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/test/LArFCalTowerBuilderTool_test.sh
                LOG_IGNORE_PATTERN "Reading file|Unable to locate catalog|Cache alignment"
                PROPERTIES TIMEOUT 600 )

atlas_add_test( LArRecUtilsConfig_test
                SCRIPT python -m LArRecUtils.LArRecUtilsConfig
                LOG_SELECT_PATTERN "ComponentAccumulator|^---" )

atlas_add_test( LArADC2MeVCondAlgConfig_test
                SCRIPT python -m LArRecUtils.LArADC2MeVCondAlgConfig
                LOG_SELECT_PATTERN "ComponentAccumulator|^---" )
