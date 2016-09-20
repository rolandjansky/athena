################################################################################
# Package: TrigEgammaRec
################################################################################

# Declare the package name:
atlas_subdir( TrigEgammaRec )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Event/xAOD/xAODEgamma
                          Event/xAOD/xAODPrimitives
                          GaudiKernel
                          LumiBlock/LumiBlockComps
                          Reconstruction/RecoTools/RecoToolInterfaces
                          Reconstruction/egamma/egammaInterfaces
                          Reconstruction/egamma/egammaRecEvent
                          Trigger/TrigEvent/TrigCaloEvent
                          Trigger/TrigEvent/TrigParticle
                          Trigger/TrigEvent/TrigSteeringEvent
                          Trigger/TrigSteer/TrigInterfaces
                          PRIVATE
                          Calorimeter/CaloEvent
                          Event/xAOD/xAODCaloEvent
                          Event/xAOD/xAODEgammaCnv
                          Event/xAOD/xAODEventShape
                          Event/xAOD/xAODTracking
                          PhysicsAnalysis/ElectronPhotonID/ElectronPhotonSelectorTools
                          Reconstruction/egamma/egammaEvent
                          Trigger/TrigTools/TrigTimeAlgs )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( TrigEgammaRecLib
                   src/*.cxx
                   PUBLIC_HEADERS TrigEgammaRec
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES xAODEgamma xAODPrimitives GaudiKernel RecoToolInterfaces egammaRecEvent TrigCaloEvent TrigParticle TrigSteeringEvent LumiBlockCompsLib TrigInterfacesLib ElectronPhotonSelectorToolsLib TrigTimeAlgsLib
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} CaloEvent xAODCaloEvent xAODEventShape xAODTracking egammaEvent )

atlas_add_component( TrigEgammaRec
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} xAODEgamma xAODPrimitives GaudiKernel LumiBlockCompsLib RecoToolInterfaces egammaRecEvent TrigCaloEvent TrigParticle TrigSteeringEvent TrigInterfacesLib CaloEvent xAODCaloEvent xAODEventShape xAODTracking ElectronPhotonSelectorToolsLib egammaEvent TrigTimeAlgsLib TrigEgammaRecLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

