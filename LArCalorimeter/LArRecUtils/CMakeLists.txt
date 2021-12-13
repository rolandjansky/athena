# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( LArRecUtils )

# External dependencies:
find_package( Boost )
find_package( CLHEP )
find_package( Eigen )
find_package( CORAL COMPONENTS CoralBase )
find_package( ROOT )

# Component(s) in the package:
atlas_add_library( LArRecUtilsLib
                   src/*.cxx
                   PUBLIC_HEADERS LArRecUtils
                   PRIVATE_INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                   PRIVATE_DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES AthAllocators AthenaBaseComps AthenaKernel CaloIdentifier CaloUtilsLib CxxUtils LArCOOLConditions LArElecCalib LArIdentifier LArRawEvent LArRecConditions LArRecEvent StoreGateLib CaloTriggerToolLib
                   PRIVATE_LINK_LIBRARIES ${Boost_LIBRARIES} ${CLHEP_LIBRARIES} ${CORAL_LIBRARIES} ${EIGEN_LIBRARIES} ${ROOT_LIBRARIES} AthenaPoolUtilities CaloConditions CaloDetDescrLib CaloEvent CaloGeoHelpers CaloInterfaceLib GaudiKernel Identifier LArCablingLib LArHV LArRawConditions LArRawUtilsLib LArReadoutGeometry PathResolver SGTools TestTools )

atlas_add_component( LArRecUtils
                     src/components/*.cxx
                     LINK_LIBRARIES LArRecUtilsLib )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} --extend-ignore=F401,F821 )

# Test(s) in the package:
atlas_add_test( LArFCalTowerBuilderTool
                SCRIPT test/LArFCalTowerBuilderTool_test.sh
                LOG_IGNORE_PATTERN "Reading file|Unable to locate catalog|Cache alignment|IOVDbSvc +INFO"
                PROPERTIES TIMEOUT 600 )

atlas_add_test( LArRecUtilsConfig_test
                SCRIPT python -m LArRecUtils.LArRecUtilsConfig
                LOG_SELECT_PATTERN "ComponentAccumulator|^---" )

atlas_add_test( LArADC2MeVCondAlgConfig_test
                SCRIPT python -m LArRecUtils.LArADC2MeVCondAlgConfig
                LOG_SELECT_PATTERN "ComponentAccumulator|^---" )

atlas_add_test( LArADC2MeVSCCondAlgConfig_test
                SCRIPT python -m LArRecUtils.LArADC2MeVSCCondAlgConfig
                POST_EXEC_SCRIPT nopost.sh )
