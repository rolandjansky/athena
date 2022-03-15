# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TileMonitoring )

# External dependencies:
find_package( CLHEP )
find_package( ROOT COMPONENTS Graf Hist HistPainter Spectrum Core Tree MathCore RIO )

# Component(s) in the package:
atlas_add_library( TileMonitoringLib
                   TileMonitoring/*.h
                   INTERFACE
                   PUBLIC_HEADERS TileMonitoring
                   LINK_LIBRARIES GaudiKernel )

atlas_add_component( TileMonitoring
  TileMonitoring/*.h src/*.cxx src/components/*.cxx
  INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
  LINK_LIBRARIES ${CLHEP_LIBRARIES} ${ROOT_LIBRARIES} AsgTools AthenaKernel AthenaMonitoringKernelLib AthenaMonitoringLib ByteStreamCnvSvcBaseLib CaloEvent CaloGeoHelpers CaloIdentifier GaudiKernel Identifier JetInterface JetMomentToolsLib JetSelectorToolsLib JetUtils LWHists StoreGateLib TileCalibBlobObjs TileConditionsLib TileEvent TileIdentifier TileMonitoringLib TileRecUtilsLib TrigConfData TrigDecisionToolLib TrigT1Result xAODCaloEvent xAODCore xAODEventInfo xAODJet xAODMuon xAODTrigger )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/*.py )
atlas_install_data( data/*.yaml )

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

atlas_add_test( TileTMDBMonitorAlgorithm_test
                SCRIPT python -m TileMonitoring.TileTMDBMonitorAlgorithm
                PROPERTIES TIMEOUT 600
                POST_EXEC_SCRIPT nopost.sh)

atlas_add_test( TileTMDBDigitsMonitorAlgorithm_test
                SCRIPT python -m TileMonitoring.TileTMDBDigitsMonitorAlgorithm
                PROPERTIES TIMEOUT 600
                POST_EXEC_SCRIPT nopost.sh)

atlas_add_test( TileTMDBRawChannelMonitorAlgorithm_test
                SCRIPT python -m TileMonitoring.TileTMDBRawChannelMonitorAlgorithm
                PROPERTIES TIMEOUT 600
                POST_EXEC_SCRIPT nopost.sh)

atlas_add_test( TileMonitoringConfig_test
                SCRIPT python -m TileMonitoring.TileMonitoringConfig
                PROPERTIES TIMEOUT 300
                POST_EXEC_SCRIPT nopost.sh)

atlas_add_test( RunTileMonitoring_test
                SCRIPT python -m TileMonitoring.RunTileMonitoring --evtMax=3
                PROPERTIES TIMEOUT 600
                POST_EXEC_SCRIPT nopost.sh)

atlas_add_test( TileDigitsMonitorAlgorithm_test
                SCRIPT python -m TileMonitoring.TileDigitsMonitorAlgorithm
                PROPERTIES TIMEOUT 600
                POST_EXEC_SCRIPT nopost.sh)
