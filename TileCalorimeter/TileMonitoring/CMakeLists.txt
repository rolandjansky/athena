################################################################################
# Package: TileMonitoring
################################################################################

# Declare the package name:
atlas_subdir( TileMonitoring )

# External dependencies:
find_package( CLHEP )
find_package( ROOT COMPONENTS Graf Gpad Hist Spectrum Core Tree MathCore
  RIO pthread Graf3d Html Postscript Gui GX11TTF GX11 HistPainter )

atlas_add_library( TileMonitoringLib
                   TileMonitoring/*.h
                   INTERFACE
                   PUBLIC_HEADERS TileMonitoring
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} 
                   LINK_LIBRARIES GaudiKernel TileEvent StoreGateLib TileConditionsLib xAODJet AsgTools JetInterface JetSelectorToolsLib AthenaMonitoringLib TileIdentifier )

# Component(s) in the package:
atlas_add_component( TileMonitoring
  TileMonitoring/*.h src/*.cxx src/components/*.cxx
  INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
  LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} 
  TileMonitoringLib
  CaloEvent CaloGeoHelpers CaloIdentifier AthenaKernel
  ByteStreamCnvSvcBaseLib xAODCaloEvent xAODEventInfo xAODTrigger xAODMuon JetUtils
  TileCalibBlobObjs TileRecUtilsLib LWHists
  TrigDecisionToolLib TrigConfL1Data TrigT1Result JetMomentToolsLib )

# Install files from the package:
atlas_install_headers( TileMonitoring )
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/*.py )

# Tests:
atlas_add_test( TileCellMonitorAlgorithm_test
                SCRIPT python -m TileMonitoring.TileCellMonitorAlgorithm
                PROPERTIES TIMEOUT 300
                POST_EXEC_SCRIPT nopost.sh)

atlas_add_test( TileTowerMonitorAlgorithm_test
                SCRIPT python -m TileMonitoring.TileTowerMonitorAlgorithm
                PROPERTIES TIMEOUT 300
                POST_EXEC_SCRIPT nopost.sh)

atlas_add_test( TileClusterMonitorAlgorithm_test
                SCRIPT python -m TileMonitoring.TileClusterMonitorAlgorithm
                PROPERTIES TIMEOUT 300
                POST_EXEC_SCRIPT nopost.sh)

atlas_add_test( TileMuIdMonitorAlgorithm_test
                SCRIPT python -m TileMonitoring.TileMuIdMonitorAlgorithm
                PROPERTIES TIMEOUT 300
                POST_EXEC_SCRIPT nopost.sh)

atlas_add_test( TileJetMonitorAlgorithm_test
                SCRIPT python -m TileMonitoring.TileJetMonitorAlgorithm
                PROPERTIES TIMEOUT 300
                POST_EXEC_SCRIPT nopost.sh)

atlas_add_test( TileDQFragMonitorAlgorithm_test
                SCRIPT python -m TileMonitoring.TileDQFragMonitorAlgorithm
                PROPERTIES TIMEOUT 300
                POST_EXEC_SCRIPT nopost.sh)

atlas_add_test( TileMBTSMonitorAlgorithm_test
                SCRIPT python -m TileMonitoring.TileMBTSMonitorAlgorithm
                PROPERTIES TIMEOUT 300
                POST_EXEC_SCRIPT nopost.sh)

atlas_add_test( TileDigiNoiseMonitorAlgorithm_test
                SCRIPT python -m TileMonitoring.TileDigiNoiseMonitorAlgorithm
                PROPERTIES TIMEOUT 300
                POST_EXEC_SCRIPT nopost.sh)

atlas_add_test( TileRawChannelTimeMonitorAlgorithm_test
                SCRIPT python -m TileMonitoring.TileRawChannelTimeMonitorAlgorithm
                PROPERTIES TIMEOUT 300
                POST_EXEC_SCRIPT nopost.sh)

atlas_add_test( TileRawChannelNoiseMonitorAlgorithm_test
                SCRIPT python -m TileMonitoring.TileRawChannelNoiseMonitorAlgorithm
                PROPERTIES TIMEOUT 600
                POST_EXEC_SCRIPT nopost.sh)

atlas_add_test( TileMuonFitMonitorAlgorithm_test
                SCRIPT python -m TileMonitoring.TileMuonFitMonitorAlgorithm
                PROPERTIES TIMEOUT 600
                POST_EXEC_SCRIPT nopost.sh)

atlas_add_test( TileRODMonitorAlgorithm_test
                SCRIPT python -m TileMonitoring.TileRODMonitorAlgorithm
                PROPERTIES TIMEOUT 600
                POST_EXEC_SCRIPT nopost.sh)

atlas_add_test( TileMonitoringConfig_test
                SCRIPT python -m TileMonitoring.TileMonitoringConfig
                PROPERTIES TIMEOUT 300
                POST_EXEC_SCRIPT nopost.sh)
