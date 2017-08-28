################################################################################
# Package: TruthD3PDMaker
################################################################################

# Declare the package name:
atlas_subdir( TruthD3PDMaker )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Control/Navigation
                          Control/SGTools
                          Event/EventInfo
                          Event/xAOD/xAODTruth
                          GaudiKernel
                          Generators/GeneratorObjects
                          Generators/GenInterfaces
                          Generators/TruthUtils
                          PhysicsAnalysis/D3PDMaker/D3PDMakerInterfaces
                          PhysicsAnalysis/D3PDMaker/D3PDMakerUtils
                          PhysicsAnalysis/D3PDMaker/TruthD3PDAnalysis
                          PhysicsAnalysis/MCTruthClassifier
                          PhysicsAnalysis/TruthParticleID/McParticleEvent
                          PhysicsAnalysis/TruthParticleID/McParticleKernel
                          Reconstruction/Jet/JetEvent
                          Tracking/TrkTools/TrkToolInterfaces )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( CLHEP )
find_package( HepMC )
find_package( HepPDT )

# Component(s) in the package:
atlas_add_component( TruthD3PDMaker
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${HEPPDT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${HEPMC_INCLUDE_DIRS}
                     LINK_LIBRARIES ${Boost_LIBRARIES} ${HEPPDT_LIBRARIES} ${CLHEP_LIBRARIES} ${HEPMC_LIBRARIES} AthenaBaseComps AthenaKernel Navigation SGTools EventInfo xAODTruth GaudiKernel GeneratorObjects TruthUtils D3PDMakerUtils TruthD3PDAnalysisLib MCTruthClassifierLib McParticleEvent McParticleKernel JetEvent TrkToolInterfaces )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

