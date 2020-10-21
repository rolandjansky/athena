# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( BeamHaloGenerator )

# External dependencies:
find_package( CLHEP )
find_package( HepPDT )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( BeamHaloGeneratorLib
                   src/*.cxx
                   PUBLIC_HEADERS BeamHaloGenerator
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} 
                   PRIVATE_INCLUDE_DIRS ${HEPPDT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                   PRIVATE_DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${ROOT_LIBRARIES} AtlasHepMCLib GaudiKernel GeneratorModulesLib
                   PRIVATE_LINK_LIBRARIES ${HEPPDT_LIBRARIES} ${CLHEP_LIBRARIES} AthenaKernel )

atlas_add_component( BeamHaloGenerator
                     src/components/*.cxx
                     LINK_LIBRARIES BeamHaloGeneratorLib )

# Install files from the package:
atlas_install_joboptions( share/*.py )

