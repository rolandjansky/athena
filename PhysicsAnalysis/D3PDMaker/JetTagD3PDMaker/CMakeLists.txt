################################################################################
# Package: JetTagD3PDMaker
################################################################################

# Declare the package name:
atlas_subdir( JetTagD3PDMaker )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( CLHEP )

# Component(s) in the package:
atlas_add_component( JetTagD3PDMaker
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${Boost_INCLUDE_DIRS}  ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${Boost_LIBRARIES} AtlasHepMCLib AthenaBaseComps AthenaKernel AthContainers EventKernel EventPrimitives GaudiKernel GeneratorObjects InDetIdentifier InDetReadoutGeometry D3PDMakerUtils JetTagEvent JetTagInfo MuonIDEvent McParticleEvent JetEvent muonEvent Particle egammaEvent TrkParticleBase VxJetVertex VxSecVertex VxVertex TrkVertexFitterInterfaces InDetConditionsSummaryService )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

