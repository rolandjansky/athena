# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TileRecUtils )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( CLHEP )

# Component(s) in the package:
atlas_add_library( TileRecUtilsLib
                   src/Tile*.cxx
                   PUBLIC_HEADERS TileRecUtils
                   INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
                   DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${CLHEP_LIBRARIES} CaloConditions CaloIdentifier AthenaBaseComps AthenaKernel Identifier GaudiKernel TileEvent TileIdentifier TileConditionsLib CaloInterfaceLib CaloUtilsLib StoreGateLib TrigAnalysisInterfaces xAODEventInfo
                   PRIVATE_LINK_LIBRARIES ${Boost_LIBRARIES} AthAllocators CaloDetDescrLib CaloEvent CxxUtils EventContainers GeoModelInterfaces PathResolver TileCalibBlobObjs TileDetDescr TileSimEvent )

atlas_add_component( TileRecUtils
                     src/components/*.cxx
                     INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES TileRecUtilsLib )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/*.py )

atlas_add_dictionary( TileRecUtilsDict
                      TileRecUtils/TileRecUtilsDict.h
                      TileRecUtils/selection.xml
                      LINK_LIBRARIES TileRecUtilsLib )

# Tests:
atlas_add_test( TileCellBuilder_test
  SCRIPT test/TileCellBuilder_test.sh
  PROPERTIES TIMEOUT 600
  LOG_IGNORE_PATTERN "LArDetectorToolNV|is still valid|no data retrieved|Database being retired|Reading file|Unable to locate catalog|Resolved path|DigitizationFlags|^Domain|created CondCont|no dictionary for class|^ +[+]|Reading LArPedestalMC|IOV callback|^DetectorStore|TileDetectorTool|Creating TileCondProxyFile|Cache alignment|No RecFlags available|IOVDbSvc +INFO" )

# The entries 
# IncrementalExecutor::executeFunction|You are probably missing the definition|Maybe you need to load
# are to work around an abi issue seen in gcc6 vs cling.
# They should not be needed with gcc8.
atlas_add_test( TileCellBuilderFromHit_test
  SCRIPT test/TileCellBuilderFromHit_test.sh
  PROPERTIES TIMEOUT 600
  LOG_IGNORE_PATTERN "LArDetectorToolNV|is still valid|no data retrieved|Database being retired|Reading file|Unable to locate catalog|Resolved path|DigitizationFlags|^Domain|created CondCont|no dictionary for class|^ +[+]|Reading LArPedestalMC|IOV callback|^DetectorStore|TileDetectorTool|Creating TileCondProxyFile|Cache alignment|IncrementalExecutor::executeFunction|You are probably missing the definition|Maybe you need to load|No RecFlags available|IOVDbSvc +INFO" )

atlas_add_test( TileRawChannelBuilder_test
  SCRIPT test/TileRawChannelBuilder_test.sh
  PROPERTIES TIMEOUT 600
  LOG_IGNORE_PATTERN "LArDetectorToolNV|is still valid|no data retrieved|Database being retired|Reading file|Unable to locate catalog|Resolved path|DigitizationFlags|^Domain|created CondCont|no dictionary for class|^ +[+]|Reading LArPedestalMC|IOV callback|^DetectorStore|TileDetectorTool|Creating TileCondProxyFile|Cache alignment|No RecFlags available|IOVDbSvc +INFO" )

atlas_add_test( TileDQstatusAlg_test
  SCRIPT test/TileDQstatusAlg_test.sh
  PROPERTIES TIMEOUT 600
  LOG_IGNORE_PATTERN "LArDetectorToolNV|is still valid|no data retrieved|Database being retired|Reading file|Unable to locate catalog|Resolved path|DigitizationFlags|^Domain|created CondCont|no dictionary for class|^ +[+]|Reading LArPedestalMC|IOV callback|^DetectorStore|TileDetectorTool|Creating TileCondProxyFile|Cache alignment|No RecFlags available|IOVDbSvc +INFO" )

atlas_add_test( TileDQstatusTool_test
  SCRIPT test/TileDQstatusTool_test.sh
  PROPERTIES TIMEOUT 600
  LOG_IGNORE_PATTERN "LArDetectorToolNV|is still valid|no data retrieved|Database being retired|Reading file|Unable to locate catalog|Resolved path|DigitizationFlags|^Domain|created CondCont|no dictionary for class|^ +[+]|Reading LArPedestalMC|IOV callback|^DetectorStore|TileDetectorTool|Creating TileCondProxyFile|Cache alignment|No RecFlags available|IOVDbSvc +INFO" )

atlas_add_test( TileRawChannelBuilderFitConfig_test
                 SCRIPT python -m TileRecUtils.TileRawChannelBuilderFitConfig
                 PROPERTIES TIMEOUT 300
                 POST_EXEC_SCRIPT nopost.sh)

atlas_add_test( TileRawChannelBuilderOptConfig_test
                 SCRIPT python -m TileRecUtils.TileRawChannelBuilderOptConfig
                 PROPERTIES TIMEOUT 300
                 POST_EXEC_SCRIPT nopost.sh)

atlas_add_test( TileRawChannelBuilderMFConfig_test
                 SCRIPT python -m TileRecUtils.TileRawChannelBuilderMFConfig
                 PROPERTIES TIMEOUT 300
                 POST_EXEC_SCRIPT nopost.sh)

atlas_add_test( TileCellNoiseFilterConfig_test
                 SCRIPT python -m TileRecUtils.TileCellNoiseFilterConfig
                 PROPERTIES TIMEOUT 300
                 POST_EXEC_SCRIPT nopost.sh)

atlas_add_test( TileCellBuilderConfig_test
                 SCRIPT python -m TileRecUtils.TileCellBuilderConfig
                 PROPERTIES TIMEOUT 300
                 POST_EXEC_SCRIPT nopost.sh)

atlas_add_test( TileDQstatusConfig_test
                 SCRIPT python -m TileRecUtils.TileDQstatusConfig
                 PROPERTIES TIMEOUT 300
                 POST_EXEC_SCRIPT nopost.sh)

atlas_add_test( TileRawChannelCorrectionConfig_test
                 SCRIPT python -m TileRecUtils.TileRawChannelCorrectionConfig
                 PROPERTIES TIMEOUT 300
                 POST_EXEC_SCRIPT nopost.sh)

atlas_add_test( TileRawChannelMakerConfig_test
                 SCRIPT python -m TileRecUtils.TileRawChannelMakerConfig
                 PROPERTIES TIMEOUT 300
                 POST_EXEC_SCRIPT nopost.sh)

atlas_add_test( TileCellMakerConfig_test
                 SCRIPT python -m TileRecUtils.TileCellMakerConfig
                 PROPERTIES TIMEOUT 300
                 POST_EXEC_SCRIPT nopost.sh)
