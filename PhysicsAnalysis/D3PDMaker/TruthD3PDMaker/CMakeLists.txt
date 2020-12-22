# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TruthD3PDMaker )

# External dependencies:
find_package( Boost )
find_package( CLHEP )
find_package( HepPDT )

# Component(s) in the package:
atlas_add_component( TruthD3PDMaker
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${HEPPDT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} 
                     LINK_LIBRARIES ${Boost_LIBRARIES} ${HEPPDT_LIBRARIES} ${CLHEP_LIBRARIES} AtlasHepMCLib AthenaBaseComps AthenaKernel Navigation EventInfo xAODTruth GaudiKernel GeneratorObjects TruthUtils D3PDMakerInterfaces D3PDMakerUtils TruthD3PDAnalysisLib MCTruthClassifierLib McParticleEvent McParticleKernel JetEvent TrkToolInterfaces GenInterfacesLib )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/*.py )

