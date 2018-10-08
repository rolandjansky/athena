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
                          Calorimeter/CaloGeoHelpers
                          Commission/CommissionEvent
                          Control/AthenaMonitoring
                          Control/StoreGate
                          DetectorDescription/Identifier
                          Event/xAOD/xAODCaloEvent
                          GaudiKernel
                          LArCalorimeter/LArIdentifier
                          LArCalorimeter/LArRecConditions
                          LArCalorimeter/LArCabling
                          Reconstruction/MissingETEvent
                          Reconstruction/egamma/egammaEvent
                          PhysicsAnalysis/ElectronPhotonID/ElectronPhotonSelectorTools
                          Tools/LWHists
                          Trigger/TrigAnalysis/TrigDecisionTool
			  Trigger/TrigAnalysis/TrigAnalysisInterfaces
                          PRIVATE
                          Calorimeter/CaloDetDescr
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
                     LINK_LIBRARIES ${ROOT_LIBRARIES} CaloEvent CaloIdentifier CommissionEvent AthenaMonitoringLib StoreGateLib SGtests Identifier xAODCaloEvent GaudiKernel LArIdentifier LArRecConditions LArCablingLib MissingETEvent egammaEvent ElectronPhotonSelectorTools LWHists TrigDecisionToolLib CaloDetDescrLib CaloGeoHelpers AthenaKernel xAODEventInfo LArRecEvent JetEvent RecBackgroundEvent )

# Install files from the package:
atlas_install_headers( CaloMonitoring )
atlas_install_joboptions( share/*.py )

#SET( CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Og " ) 
