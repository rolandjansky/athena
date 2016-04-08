################################################################################
# Package: TrigHIRec
################################################################################

# Declare the package name:
atlas_subdir( TrigHIRec )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          GaudiKernel
                          Trigger/TrigEvent/TrigCaloEvent
                          PRIVATE
                          Calorimeter/CaloEvent
                          Calorimeter/CaloUtils
                          Control/AthToolSupport/AsgTools
                          Control/Navigation
                          Control/StoreGate
                          Event/NavFourMom
                          Event/xAOD/xAODCaloEvent
                          Event/xAOD/xAODCore
                          Event/xAOD/xAODHIEvent
                          Event/xAOD/xAODJet
                          PhysicsAnalysis/HeavyIonPhys/HIEventUtils
                          Reconstruction/HeavyIonRec/HIGlobal
                          Reconstruction/HeavyIonRec/HIJetRec
                          Reconstruction/Jet/JetInterface
                          Trigger/TrigSteer/TrigInterfaces )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( TrigHIRec
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} GaudiKernel TrigCaloEvent CaloEvent CaloUtilsLib AsgTools Navigation StoreGateLib SGtests NavFourMom xAODCaloEvent xAODCore xAODHIEvent xAODJet HIEventUtilsLib JetInterface TrigInterfacesLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )

