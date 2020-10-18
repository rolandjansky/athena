################################################################################
# Package: RpcRawDataMonitoring
################################################################################

# Declare the package name:
atlas_subdir( RpcRawDataMonitoring )

# External dependencies:
find_package( Eigen )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread Graf Graf3d Gpad Html Postscript Gui GX11TTF GX11 )

# Component(s) in the package:
atlas_add_component( RpcRawDataMonitoring
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${EIGEN_LIBRARIES} AthenaMonitoringLib StoreGateLib xAODEventInfo xAODMuon xAODTrigger xAODTracking GaudiKernel RPCcablingInterfaceLib MuonReadoutGeometry MuonGeoModelLib MuonRDO MuonTrigCoinData MuonDQAUtilsLib TrigDecisionToolLib TrigT1Interfaces GeoPrimitives EventPrimitives MuonDigitContainer MuonIdHelpersLib MuonPrepRawData TrkEventPrimitives TrkMeasurementBase TrkMultiComponentStateOnSurface TrkTrack TrigConfL1Data TrigT1Result MuonAnalysisInterfacesLib )

# Install files from the package:
atlas_install_headers( RpcRawDataMonitoring )
atlas_install_joboptions( share/*.py )

atlas_install_python_modules( python/*.py )
