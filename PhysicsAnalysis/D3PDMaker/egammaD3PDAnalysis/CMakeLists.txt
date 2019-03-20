################################################################################
# Package: egammaD3PDAnalysis
################################################################################

# Declare the package name:
atlas_subdir( egammaD3PDAnalysis )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          Calorimeter/CaloClusterCorrection
                          Calorimeter/CaloEvent
                          Calorimeter/CaloGeoHelpers
                          Calorimeter/CaloInterface
                          Calorimeter/CaloRec
                          Control/AthContainers
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Control/CxxUtils
                          Event/EventKernel
                          Event/xAOD/xAODCaloEvent
                          Event/xAOD/xAODEgamma
                          Event/xAOD/xAODTruth
                          GaudiKernel
                          LArCalorimeter/LArCabling
                          PhysicsAnalysis/D3PDMaker/D3PDMakerInterfaces
                          PhysicsAnalysis/D3PDMaker/D3PDMakerUtils
                          PhysicsAnalysis/MCTruthClassifier
                          Reconstruction/RecoTools/RecoToolInterfaces
                          Reconstruction/egamma/egammaEvent
                          Reconstruction/egamma/egammaInterfaces
                          Tracking/TrkEvent/TrkCaloExtension
                          Tracking/TrkEvent/VxVertex )

# Component(s) in the package:
atlas_add_component( egammaD3PDAnalysis
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES CaloClusterCorrectionLib CaloEvent CaloGeoHelpers CaloRecLib AthContainers AthenaBaseComps AthenaKernel CxxUtils EventKernel xAODCaloEvent xAODEgamma xAODTruth GaudiKernel LArCablingLib D3PDMakerUtils MCTruthClassifierLib RecoToolInterfaces egammaEvent TrkCaloExtension VxVertex )

# Install files from the package:
atlas_install_headers( egammaD3PDAnalysis )
atlas_install_python_modules( python/*.py )

