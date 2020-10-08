################################################################################
# Package: TgcRawDataMonitoring
################################################################################

# Declare the package name:
atlas_subdir( TgcRawDataMonitoring )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread MathMore Minuit Minuit2 Matrix Physics HistPainter Rint Graf Graf3d Gpad Html Postscript Gui GX11TTF GX11 )

atlas_add_component( TgcRawDataMonitoring
                     src/*.cxx src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} GaudiKernel AthenaMonitoringLib AthenaMonitoringKernelLib
                     StoreGateLib SGtests MuonReadoutGeometry MuonIdHelpersLib MuonPrepRawData MuonTrigCoinData MuonDQAUtilsLib Identifier xAODEventInfo xAODMuon xAODTrigger MuonRDO AnalysisTriggerEvent muonEvent TrigSteeringEvent TrigDecisionToolLib TrkExInterfaces )


# Install files from the package:
atlas_install_headers( TgcRawDataMonitoring )
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

