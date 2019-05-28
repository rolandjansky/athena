################################################################################
# Package: TrigEgammaRec
################################################################################

# Declare the package name:
atlas_subdir( TrigEgammaRec )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
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
                          Calorimeter/CaloEvent
                          Calorimeter/CaloUtils
                          Event/xAOD/xAODCaloEvent
                          Event/xAOD/xAODEgammaCnv
                          Event/xAOD/xAODEventShape
                          Event/xAOD/xAODTracking
			              PhysicsAnalysis/Interfaces/EgammaAnalysisInterfaces
                          Reconstruction/egamma/egammaEvent
                          Reconstruction/egamma/egammaUtils
                          Trigger/TrigTools/TrigTimeAlgs )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( TrigEgammaRec
                     src/*.cxx src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} xAODEgamma xAODPrimitives GaudiKernel CaloUtilsLib LumiBlockCompsLib RecoToolInterfaces egammaRecEvent TrigCaloEvent TrigParticle TrigSteeringEvent TrigInterfacesLib CaloEvent xAODCaloEvent xAODEventShape xAODTracking EgammaAnalysisInterfacesLib egammaEvent egammaUtils TrigTimeAlgsLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

