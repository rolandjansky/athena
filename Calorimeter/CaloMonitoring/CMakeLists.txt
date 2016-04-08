################################################################################
# Package: CaloMonitoring
################################################################################

# Declare the package name:
atlas_subdir( CaloMonitoring )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Calorimeter/CaloEvent
                          Calorimeter/CaloIdentifier
                          Calorimeter/CaloInterface
                          Commission/CommissionEvent
                          Control/AthenaMonitoring
                          Control/StoreGate
                          DetectorDescription/Identifier
                          Event/xAOD/xAODCaloEvent
                          GaudiKernel
                          LArCalorimeter/LArIdentifier
                          LArCalorimeter/LArRecConditions
                          LArCalorimeter/LArTools
                          Reconstruction/MissingETEvent
                          Reconstruction/egamma/egammaEvent
                          Tools/LWHists
                          Trigger/TrigAnalysis/TrigDecisionTool
                          PRIVATE
                          Calorimeter/CaloDetDescr
                          Calorimeter/CaloGeoHelpers
                          Control/AthenaKernel
                          Event/xAOD/xAODEventInfo
                          LArCalorimeter/LArRecEvent
                          Reconstruction/Jet/JetEvent
                          Reconstruction/RecBackground/RecBackgroundEvent )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( CaloMonitoring
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} CaloEvent CaloIdentifier CommissionEvent AthenaMonitoringLib StoreGateLib SGtests Identifier xAODCaloEvent GaudiKernel LArIdentifier LArRecConditions LArToolsLib MissingETEvent egammaEvent LWHists TrigDecisionToolLib CaloDetDescrLib CaloGeoHelpers AthenaKernel xAODEventInfo LArRecEvent JetEvent RecBackgroundEvent )

# Install files from the package:
atlas_install_headers( CaloMonitoring )
atlas_install_joboptions( share/*.py )

