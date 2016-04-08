################################################################################
# Package: egammaD3PDMaker
################################################################################

# Declare the package name:
atlas_subdir( egammaD3PDMaker )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          Calorimeter/CaloIdentifier
                          Calorimeter/CaloTrackingGeometry
                          Control/AthenaKernel
                          Control/Navigation
                          Control/SGTools
                          Control/StoreGate
                          Event/EventKernel
                          Event/FourMomUtils
                          Event/xAOD/xAODBase
                          Event/xAOD/xAODCaloEvent
                          Event/xAOD/xAODEgamma
                          Event/xAOD/xAODJet
                          Event/xAOD/xAODTracking
                          Event/xAOD/xAODTruth
                          GaudiKernel
                          PhysicsAnalysis/AnalysisTrigger/AnalysisTriggerEvent
                          PhysicsAnalysis/D3PDMaker/D3PDMakerInterfaces
                          PhysicsAnalysis/D3PDMaker/D3PDMakerUtils
                          PhysicsAnalysis/D3PDMaker/TriggerD3PDMaker
                          PhysicsAnalysis/MCTruthClassifier
                          Reconstruction/egamma/egammaEvent
                          Tracking/TrkEvent/TrkParameters
                          Tracking/TrkExtrapolation/TrkExInterfaces
                          Tracking/TrkVertexFitter/TrkVertexFitterInterfaces
                          Trigger/TrigAnalysis/TrigObjectMatching
                          Trigger/TrigEvent/TrigCaloEvent
                          Trigger/TrigEvent/TrigInDetEvent
                          Trigger/TrigEvent/TrigParticle )

# External dependencies:
find_package( CLHEP )
find_package( HepMC )

# Component(s) in the package:
atlas_add_component( egammaD3PDMaker
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS} ${HEPMC_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} ${HEPMC_LIBRARIES} CaloIdentifier AthenaKernel Navigation SGTools StoreGateLib SGtests EventKernel FourMomUtils xAODBase xAODCaloEvent xAODEgamma xAODJet xAODTracking xAODTruth GaudiKernel AnalysisTriggerEvent D3PDMakerUtils TriggerD3PDMakerLib MCTruthClassifierLib egammaEvent TrkParameters TrkExInterfaces TrkVertexFitterInterfaces TrigObjectMatchingLib TrigCaloEvent TrigInDetEvent TrigParticle )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

