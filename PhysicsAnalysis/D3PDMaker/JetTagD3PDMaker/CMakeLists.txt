# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( JetTagD3PDMaker )

# External dependencies:
find_package( Boost )
find_package( CLHEP )

# Component(s) in the package:
atlas_add_component( JetTagD3PDMaker
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${Boost_INCLUDE_DIRS}  ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${Boost_LIBRARIES} ${CLHEP_LIBRARIES} AtlasHepMCLib AthenaBaseComps AthenaKernel AthContainers EventPrimitives GaudiKernel GeneratorObjects InDetIdentifier InDetReadoutGeometry D3PDMakerUtils JetTagEvent JetTagInfo MuonIDEvent McParticleEvent JetEvent muonEvent Particle egammaEvent StoreGateLib TrkParticleBase VxJetVertex VxSecVertex VxVertex InDetConditionsSummaryService )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/*.py )

