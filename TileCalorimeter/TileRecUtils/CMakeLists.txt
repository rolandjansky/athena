################################################################################
# Package: TileRecUtils
################################################################################

# Declare the package name:
atlas_subdir( TileRecUtils )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Calorimeter/CaloConditions
                          Calorimeter/CaloIdentifier
                          Calorimeter/CaloInterface
                          Calorimeter/CaloUtils
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          DetectorDescription/Identifier
                          GaudiKernel
                          TileCalorimeter/TileConditions
                          TileCalorimeter/TileEvent
                          TileCalorimeter/TileSimEvent
                          TileCalorimeter/TileIdentifier
                          PRIVATE
                          Control/CxxUtils
                          Calorimeter/CaloDetDescr
                          Calorimeter/CaloEvent
                          Control/AthAllocators
                          Control/StoreGate
                          Event/EventContainers
                          Event/xAOD/xAODEventInfo
                          TileCalorimeter/TileCalib/TileCalibBlobObjs
                          TileCalorimeter/TileDetDescr
                          Tools/PathResolver
                          Trigger/TrigAnalysis/TrigAnalysisInterfaces )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( ROOT COMPONENTS Core )
find_package( CLHEP )

# Component(s) in the package:
atlas_add_library( TileRecUtilsLib
                   src/Tile*.cxx
                   PUBLIC_HEADERS TileRecUtils
                   INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
                   DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${CLHEP_LIBRARIES} CaloConditions CaloIdentifier AthenaBaseComps AthenaKernel Identifier GaudiKernel TileEvent TileSimEvent TileIdentifier CaloUtilsLib TileConditionsLib CaloDetDescrLib StoreGateLib SGtests CxxUtils TrigAnalysisInterfaces
                   PRIVATE_LINK_LIBRARIES ${Boost_LIBRARIES} CaloEvent AthAllocators xAODEventInfo TileCalibBlobObjs TileDetDescr PathResolver )

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
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} TileRecUtilsLib )


atlas_add_test( TileCellBuilder_test
  SCRIPT test/TileCellBuilder_test.sh
  PROPERTIES TIMEOUT 600
  LOG_IGNORE_PATTERN "LArDetectorToolNV|is still valid|no data retrieved|Database being retired|Reading file|Unable to locate catalog|Resolved path|DigitizationFlags|^Domain|created CondCont|no dictionary for class|^ +[+]|Reading LArPedestalMC|IOV callback|^DetectorStore|TileDetectorTool|Creating TileCondProxyFile|Cache alignment|No RecFlags available" )


# The entries 
# IncrementalExecutor::executeFunction|You are probably missing the definition|Maybe you need to load
# are to work around an abi issue seen in gcc6 vs cling.
# They should not be needed with gcc8.
atlas_add_test( TileCellBuilderFromHit_test
  SCRIPT test/TileCellBuilderFromHit_test.sh
  PROPERTIES TIMEOUT 600
  LOG_IGNORE_PATTERN "LArDetectorToolNV|is still valid|no data retrieved|Database being retired|Reading file|Unable to locate catalog|Resolved path|DigitizationFlags|^Domain|created CondCont|no dictionary for class|^ +[+]|Reading LArPedestalMC|IOV callback|^DetectorStore|TileDetectorTool|Creating TileCondProxyFile|Cache alignment|IncrementalExecutor::executeFunction|You are probably missing the definition|Maybe you need to load|No RecFlags available" )


atlas_add_test( TileRawChannelBuilder_test
  SCRIPT test/TileRawChannelBuilder_test.sh
  PROPERTIES TIMEOUT 600
  LOG_IGNORE_PATTERN "LArDetectorToolNV|is still valid|no data retrieved|Database being retired|Reading file|Unable to locate catalog|Resolved path|DigitizationFlags|^Domain|created CondCont|no dictionary for class|^ +[+]|Reading LArPedestalMC|IOV callback|^DetectorStore|TileDetectorTool|Creating TileCondProxyFile|Cache alignment|No RecFlags available" )


atlas_add_test( TileDQstatusAlg_test
  SCRIPT test/TileDQstatusAlg_test.sh
  PROPERTIES TIMEOUT 600
  LOG_IGNORE_PATTERN "LArDetectorToolNV|is still valid|no data retrieved|Database being retired|Reading file|Unable to locate catalog|Resolved path|DigitizationFlags|^Domain|created CondCont|no dictionary for class|^ +[+]|Reading LArPedestalMC|IOV callback|^DetectorStore|TileDetectorTool|Creating TileCondProxyFile|Cache alignment|No RecFlags available" )


atlas_add_test( TileDQstatusTool_test
  SCRIPT test/TileDQstatusTool_test.sh
  PROPERTIES TIMEOUT 600
  LOG_IGNORE_PATTERN "LArDetectorToolNV|is still valid|no data retrieved|Database being retired|Reading file|Unable to locate catalog|Resolved path|DigitizationFlags|^Domain|created CondCont|no dictionary for class|^ +[+]|Reading LArPedestalMC|IOV callback|^DetectorStore|TileDetectorTool|Creating TileCondProxyFile|Cache alignment|No RecFlags available" )

# Tests:
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
