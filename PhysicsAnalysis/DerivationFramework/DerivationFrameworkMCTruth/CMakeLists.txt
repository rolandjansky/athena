# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( DerivationFrameworkMCTruth )

# External dependencies:
find_package( FastJet )
find_package( HepPDT )
find_package( ROOT COMPONENTS Core Hist )
find_package( Boost )

# Component(s) in the package:
atlas_add_component( DerivationFrameworkMCTruth
                     src/*.cxx src/components/*.cxx
                     INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${FASTJET_INCLUDE_DIRS} ${HEPPDT_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${Boost_LIBRARIES} ${FASTJET_LIBRARIES} ${HEPPDT_LIBRARIES} ${ROOT_LIBRARIES} AthenaBaseComps AthenaKernel AtlasHepMCLib DerivationFrameworkInterfaces ExpressionEvaluationLib GaudiKernel GeneratorObjects MCTruthClassifierLib StoreGateLib TauAnalysisToolsLib TruthUtils xAODBase xAODEgamma xAODEventInfo xAODJet xAODMuon xAODTruth )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

