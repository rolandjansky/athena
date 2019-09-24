################################################################################
# Package: TrigCaloMonitoring
################################################################################

# Declare the package name:
atlas_subdir( TrigCaloMonitoring )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          GaudiKernel
                          LArCalorimeter/LArRecEvent
                          Trigger/TrigMonitoring/TrigHLTMonitoring
                          PRIVATE
                          Calorimeter/CaloEvent
                          Calorimeter/CaloGeoHelpers
                          Calorimeter/CaloIdentifier
                          Calorimeter/CaloInterface
                          Control/StoreGate
                          LArCalorimeter/LArIdentifier
                          LArCalorimeter/LArCabling
                          Reconstruction/egamma/egammaEvent
                          Trigger/TrigAlgorithms/TrigCaloRec
                          Trigger/TrigAlgorithms/TrigT2CaloCommon
                          Trigger/TrigEvent/TrigCaloEvent
                          Trigger/TrigEvent/TrigSteeringEvent 
                          Control/AthenaMonitoring )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread MathMore Minuit Minuit2 Matrix Physics HistPainter Rint Graf Graf3d Gpad Html Postscript Gui GX11TTF GX11 )

# Component(s) in the package:
atlas_add_component( TrigCaloMonitoring
                     src/*.cxx src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} GaudiKernel LArRecEvent TrigHLTMonitoringLib StoreGateLib 
                     LArCablingLib TrigCaloRecLib TrigT2CaloCommonLib CaloEvent CaloGeoHelpers CaloIdentifier 
                     LArIdentifier egammaEvent TrigCaloEvent TrigSteeringEvent AthenaMonitoringLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )
atlas_install_headers( TrigCaloMonitoring )

