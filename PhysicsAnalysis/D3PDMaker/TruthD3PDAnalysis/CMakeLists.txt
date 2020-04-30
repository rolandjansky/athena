################################################################################
# Package: TruthD3PDAnalysis
################################################################################

# Declare the package name:
atlas_subdir( TruthD3PDAnalysis )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          GaudiKernel
                          PhysicsAnalysis/D3PDMaker/D3PDMakerUtils
                          PhysicsAnalysis/TruthParticleID/McParticleKernel
                          PRIVATE
                          Control/AthenaKernel
                          Control/StoreGate
                          Event/EventKernel
                          Event/xAOD/xAODEgamma
                          Event/xAOD/xAODJet
                          Event/xAOD/xAODMuon
                          Generators/GenAnalysisTools/TruthHelper
                          Generators/GeneratorObjects
                          Generators/AtlasHepMC
                          PhysicsAnalysis/AnalysisCommon/ParticleJetTools
                          PhysicsAnalysis/MCTruthClassifier
                          PhysicsAnalysis/TruthParticleID/McParticleEvent
                          Reconstruction/Particle
                          Reconstruction/ParticleTruth
                          Tracking/TrkEvent/TrkParticleBase
                          Tracking/TrkEvent/VxVertex )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( HepPDT )

# Component(s) in the package:
atlas_add_library( TruthD3PDAnalysisLib
                   src/*.cxx
                   PUBLIC_HEADERS TruthD3PDAnalysis
                   PRIVATE_INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${HEPPDT_INCLUDE_DIRS} 
                   LINK_LIBRARIES AthenaBaseComps GaudiKernel D3PDMakerUtils McParticleKernel StoreGateLib SGtests ParticleJetToolsLib MCTruthClassifierLib MCTruthClassifierLib
                   PRIVATE_LINK_LIBRARIES ${Boost_LIBRARIES} ${HEPPDT_LIBRARIES} AtlasHepMCLib AthenaKernel EventKernel xAODEgamma xAODJet xAODMuon TruthHelper GeneratorObjects McParticleEvent Particle ParticleTruth TrkParticleBase VxVertex )

atlas_add_component( TruthD3PDAnalysis
                     src/components/*.cxx
                     INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${HEPPDT_INCLUDE_DIRS} 
                     LINK_LIBRARIES ${Boost_LIBRARIES} ${HEPPDT_LIBRARIES} AtlasHepMCLib AthenaBaseComps GaudiKernel D3PDMakerUtils McParticleKernel AthenaKernel StoreGateLib SGtests EventKernel xAODEgamma xAODJet xAODMuon TruthHelper GeneratorObjects ParticleJetToolsLib MCTruthClassifierLib McParticleEvent Particle ParticleTruth TrkParticleBase VxVertex TruthD3PDAnalysisLib MCTruthClassifierLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )

