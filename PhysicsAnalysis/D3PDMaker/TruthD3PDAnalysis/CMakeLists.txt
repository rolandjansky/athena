# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TruthD3PDAnalysis )

# External dependencies:
find_package( Boost )
find_package( HepPDT )

# Component(s) in the package:
atlas_add_library( TruthD3PDAnalysisLib
                   src/*.cxx
                   PUBLIC_HEADERS TruthD3PDAnalysis
                   PRIVATE_INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${HEPPDT_INCLUDE_DIRS}
                   LINK_LIBRARIES AthenaBaseComps AtlasHepMCLib AtlasHepMCsearchLib D3PDMakerUtils GaudiKernel McParticleKernel MCTruthClassifierLib
                   PRIVATE_LINK_LIBRARIES ${Boost_LIBRARIES} ${HEPPDT_LIBRARIES} AthenaKernel EventKernel xAODEgamma xAODJet xAODMuon TruthHelper GeneratorObjects McParticleEvent Particle ParticleJetToolsLib ParticleTruth StoreGateLib TrkParticleBase VxVertex )

atlas_add_component( TruthD3PDAnalysis
                     src/components/*.cxx
                     LINK_LIBRARIES TruthD3PDAnalysisLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )

