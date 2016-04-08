################################################################################
# Package: JetTagD3PDMaker
################################################################################

# Declare the package name:
atlas_subdir( JetTagD3PDMaker )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Control/CLIDSvc
                          Control/DataModel
                          Control/SGTools
                          Event/EventKernel
                          Event/EventPrimitives
                          GaudiKernel
                          Generators/GeneratorObjects
                          InnerDetector/InDetConditions/InDetConditionsSummaryService
                          InnerDetector/InDetDetDescr/InDetIdentifier
                          InnerDetector/InDetDetDescr/InDetReadoutGeometry
                          PhysicsAnalysis/D3PDMaker/D3PDMakerUtils
                          PhysicsAnalysis/JetTagging/JetTagEvent
                          PhysicsAnalysis/JetTagging/JetTagInfo
                          PhysicsAnalysis/MuonID/MuonIDEvent
                          PhysicsAnalysis/TruthParticleID/McParticleEvent
                          Reconstruction/Jet/JetEvent
                          Reconstruction/MuonIdentification/muonEvent
                          Reconstruction/Particle
                          Reconstruction/egamma/egammaEvent
                          Tracking/TrkEvent/TrkParticleBase
                          Tracking/TrkEvent/VxJetVertex
                          Tracking/TrkEvent/VxSecVertex
                          Tracking/TrkEvent/VxVertex
                          Tracking/TrkVertexFitter/TrkVertexFitterInterfaces )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( HepMC )

# Component(s) in the package:
atlas_add_component( JetTagD3PDMaker
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${HEPMC_INCLUDE_DIRS}
                     LINK_LIBRARIES ${Boost_LIBRARIES} ${HEPMC_LIBRARIES} AthenaBaseComps AthenaKernel DataModel SGTools EventKernel EventPrimitives GaudiKernel GeneratorObjects InDetIdentifier InDetReadoutGeometry D3PDMakerUtils JetTagEvent JetTagInfo MuonIDEvent McParticleEvent JetEvent muonEvent Particle egammaEvent TrkParticleBase VxJetVertex VxSecVertex VxVertex TrkVertexFitterInterfaces )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

