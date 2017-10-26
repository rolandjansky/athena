################################################################################
# Package: TrigHLTJetRec
################################################################################

# Declare the package name:
atlas_subdir( TrigHLTJetRec )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/StoreGate
                          Event/xAOD/xAODBase
                          Event/xAOD/xAODCaloEvent
                          Event/xAOD/xAODJet
                          Event/xAOD/xAODTrigL1Calo
                          GaudiKernel
                          Reconstruction/Jet/JetEDM
                          Reconstruction/Jet/JetInterface
                          Reconstruction/Jet/JetRec
                          Trigger/TrigEvent/TrigCaloEvent
                          Trigger/TrigEvent/TrigNavStructure
                          Trigger/TrigEvent/TrigParticle
                          Trigger/TrigEvent/TrigSteeringEvent
                          Trigger/TrigSteer/TrigInterfaces
                          PRIVATE
                          Calorimeter/CaloEvent
                          Event/xAOD/xAODEventShape
                          Reconstruction/EventShapes/EventShapeTools
                          Reconstruction/Jet/JetRecTools 
                          InnerDetector/InDetRecTools/TrackVertexAssociationTool )

# External dependencies:
find_package( FastJet )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( TrigHLTJetRec
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${FASTJET_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${FASTJET_LIBRARIES} AthenaBaseComps StoreGateLib SGtests xAODBase xAODCaloEvent xAODJet xAODTrigL1Calo GaudiKernel JetEDM JetInterface JetRecLib TrigCaloEvent TrigNavStructure TrigParticle TrigSteeringEvent TrigInterfacesLib CaloEvent xAODEventShape EventShapeToolsLib JetRecToolsLib TrackVertexAssociationToolLib  )

# Install files from the package:
atlas_install_headers( TrigHLTJetRec )
atlas_install_python_modules( python/*.py )

