################################################################################
# Package: TrigMinBiasMonitoring
################################################################################

# Declare the package name:
atlas_subdir( TrigMinBiasMonitoring )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Calorimeter/CaloIdentifier
                          Control/AthenaMonitoring
                          Event/xAOD/xAODEventInfo
                          ForwardDetectors/ZDC/ZdcIdentifier
                          GaudiKernel
                          InnerDetector/InDetRecTools/InDetTrackSelectionTool
                          Trigger/TrigAnalysis/TrigDecisionTool
                          Trigger/TrigMonitoring/TrigHLTMonitoring
                          PRIVATE
                          Calorimeter/CaloEvent
                          Calorimeter/CaloGeoHelpers
                          Event/xAOD/xAODTracking
                          Event/xAOD/xAODTrigMinBias
                          ForwardDetectors/LUCID/LUCID_RawEvent
                          ForwardDetectors/ZDC/ZdcEvent
                          InnerDetector/InDetRawEvent/InDetBCM_RawData
                          TileCalorimeter/TileEvent
                          Trigger/TrigEvent/TrigCaloEvent
                          Trigger/TrigEvent/TrigInDetEvent )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread MathMore Minuit Minuit2 Matrix Physics HistPainter Rint Graf Graf3d Gpad Html Postscript Gui GX11TTF GX11 )

# Component(s) in the package:
atlas_add_library( TrigMinBiasMonitoringLib
                   src/*.cxx
                   PUBLIC_HEADERS TrigMinBiasMonitoring
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${ROOT_LIBRARIES} CaloIdentifier xAODEventInfo ZdcIdentifier GaudiKernel AthenaMonitoringLib TrigDecisionToolLib TrigHLTMonitoringLib
                   PRIVATE_LINK_LIBRARIES CaloEvent CaloGeoHelpers xAODTracking xAODTrigMinBias LUCID_RawEvent ZdcEvent InDetBCM_RawData TileEvent TrigCaloEvent TrigInDetEvent InDetTrackSelectionToolLib )

atlas_add_component( TrigMinBiasMonitoring
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} CaloIdentifier AthenaMonitoringLib xAODEventInfo ZdcIdentifier GaudiKernel TrigDecisionToolLib TrigHLTMonitoringLib CaloEvent CaloGeoHelpers xAODTracking xAODTrigMinBias LUCID_RawEvent ZdcEvent InDetBCM_RawData TileEvent TrigCaloEvent TrigInDetEvent TrigMinBiasMonitoringLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )

