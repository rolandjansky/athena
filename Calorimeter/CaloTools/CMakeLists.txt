# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( CaloTools )

# External dependencies:
find_package( Boost )
find_package( CLHEP )
find_package( CORAL COMPONENTS CoralBase )
find_package( ROOT COMPONENTS Core MathCore )

# Component(s) in the package:
atlas_add_component( CaloTools
   CaloTools/*.h CaloTools/*.icc src/*.cxx src/components/*.cxx
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
   LINK_LIBRARIES ${Boost_LIBRARIES} ${CLHEP_LIBRARIES} ${CORAL_LIBRARIES} ${ROOT_LIBRARIES} AthenaBaseComps AthenaKernel AthenaPoolUtilities AtlasDetDescr CaloCondBlobObjs CaloConditions CaloDetDescrLib CaloEvent CaloGeoHelpers CaloIdentifier CaloInterfaceLib CxxUtils GaudiKernel Identifier LArCablingLib LArElecCalib LArRawConditions LArRecEvent StoreGateLib TileConditionsLib TileEvent TileIdentifier xAODBase )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

atlas_add_test( CaloEstimatedGainToolConfig_test
                SCRIPT python -m CaloTools.CaloEstimatedGainToolConfig
                LOG_SELECT_PATTERN "ComponentAccumulator|^---|^IOVDbSvc" )

atlas_add_test( CaloEstimatedGainTool_test
                SCRIPT python -m CaloTools.CaloEstimatedGainTool_test
                PROPERTIES TIMEOUT 300
                LOG_SELECT_PATTERN "ERROR|error|WARNING [^U]|FATAL|processing|TestAlg" )

