# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( GeneratorFilters )

# External dependencies:
find_package( CLHEP )
find_package( HepPDT )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread MathMore Minuit Minuit2 Matrix Physics HistPainter Rint )

# Component(s) in the package:
atlas_add_library( GeneratorFiltersLib
                   src/*.cxx
                   PUBLIC_HEADERS GeneratorFilters
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} 
                   PRIVATE_INCLUDE_DIRS ${HEPPDT_INCLUDE_DIRS}
                   DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} AtlasHepMCLib GaudiKernel StoreGateLib GeneratorModulesLib xAODJet
                   PRIVATE_LINK_LIBRARIES ${HEPPDT_LIBRARIES} AthenaKernel CxxUtils EventInfo xAODTruth TruthUtils ParticleEvent McParticleEvent )

atlas_add_component( GeneratorFilters
                     src/components/*.cxx
                     LINK_LIBRARIES GeneratorFiltersLib )

# Install job option fragments from the package:
atlas_install_joboptions( share/common/*.py )

