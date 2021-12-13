################################################################################
# Package: RpcRawDataMonitoring
################################################################################

# Declare the package name:
atlas_subdir( RpcRawDataMonitoring )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread Graf Graf3d Gpad Html Postscript Gui GX11TTF GX11 )

# Component(s) in the package:
atlas_add_component( RpcRawDataMonitoring
                     src/*.cxx
                     src/components/*.cxx

                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES}
                     AthenaMonitoringLib AthenaMonitoringKernelLib
                     StoreGateLib SGtests
                     LumiBlockData
                     xAODEventInfo xAODMuon xAODTrigger xAODTracking GaudiKernel MuonReadoutGeometry MuonGeoModelLib MuonRDO MuonRIO_OnTrack MuonRPC_CablingLib MuonTrigCoinData MuonDQAUtilsLib muonEvent 
                     FourMomUtils
                     GeoPrimitives EventPrimitives 
                     Identifier
                     AnalysisTriggerEvent
                     MuonDigitContainer MuonIdHelpersLib MuonPrepRawData 
                     TrigDecisionToolLib TrigT1Interfaces TrigSteeringEvent TrigConfL1Data TrigT1Result 
                     TrkEventPrimitives TrkMeasurementBase TrkMultiComponentStateOnSurface TrkTrack TrkExInterfaces
                     MuonAnalysisInterfacesLib RPC_CondCablingLib )

# Install files from the package:
atlas_install_headers( RpcRawDataMonitoring )
atlas_install_joboptions( share/*.py )
atlas_install_joboptions( python/*.py )

atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
