################################################################################
# Package: TrigHLTJetHemisphereRec
################################################################################

# Declare the package name:
atlas_subdir( TrigHLTJetHemisphereRec )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Event/xAOD/xAODJet
                          GaudiKernel
                          Trigger/TrigEvent/TrigCaloEvent
                          Trigger/TrigEvent/TrigParticle
                          Trigger/TrigEvent/TrigSteeringEvent
                          Trigger/TrigSteer/TrigInterfaces
                          Trigger/TrigTools/TrigTimeAlgs
                          PRIVATE
                          Event/xAOD/xAODCaloEvent
                          Reconstruction/Jet/JetEDM )

# External dependencies:
find_package( FastJet )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( TrigHLTJetHemisphereRec
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${FASTJET_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${FASTJET_LIBRARIES} xAODJet GaudiKernel TrigCaloEvent TrigParticle TrigSteeringEvent TrigInterfacesLib TrigTimeAlgsLib xAODCaloEvent JetEDM )

# Install files from the package:
atlas_install_headers( TrigHLTJetHemisphereRec )
atlas_install_python_modules( python/*.py )

