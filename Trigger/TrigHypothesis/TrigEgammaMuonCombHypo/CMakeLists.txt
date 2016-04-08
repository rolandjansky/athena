################################################################################
# Package: TrigEgammaMuonCombHypo
################################################################################

# Declare the package name:
atlas_subdir( TrigEgammaMuonCombHypo )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/StoreGate
                          DetectorDescription/GeoPrimitives
                          GaudiKernel
                          Reconstruction/egamma/egammaEvent
                          Trigger/TrigEvent/TrigCaloEvent
                          Trigger/TrigEvent/TrigInDetEvent
                          Trigger/TrigEvent/TrigMuonEvent
                          Trigger/TrigEvent/TrigParticle
                          Trigger/TrigEvent/TrigSteeringEvent
                          Trigger/TrigEvent/TrigTopoEvent
                          Trigger/TrigSteer/TrigInterfaces
                          PRIVATE
                          Control/CxxUtils
                          Event/xAOD/xAODEgamma
                          Event/xAOD/xAODMuon
                          Reconstruction/MuonIdentification/MuidEvent
                          Reconstruction/RecoTools/ITrackToVertex
                          Tracking/TrkEvent/VxVertex )

# External dependencies:
find_package( Eigen )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( TrigEgammaMuonCombHypo
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${EIGEN_LIBRARIES} StoreGateLib SGtests GeoPrimitives GaudiKernel egammaEvent TrigCaloEvent TrigInDetEvent TrigMuonEvent TrigParticle TrigSteeringEvent TrigTopoEvent TrigInterfacesLib CxxUtils xAODEgamma xAODMuon MuidEvent ITrackToVertex VxVertex )

# Install files from the package:
atlas_install_headers( TrigEgammaMuonCombHypo )
atlas_install_python_modules( python/*.py )

