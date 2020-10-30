# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( CosmicGenerator )

# External dependencies:
find_package( CLHEP )

# Component(s) in the package:
atlas_add_library( CosmicGeneratorLib
                   src/cosmic2.f
                   src/CosmicGun.cxx
                   src/CosmicGenerator.cxx
                   PUBLIC_HEADERS CosmicGenerator
                   INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS} 
                   DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${CLHEP_LIBRARIES} AtlasHepMCLib AthenaKernel GeneratorModulesLib
                   PRIVATE_LINK_LIBRARIES GaudiKernel )

atlas_add_component( CosmicGenerator
                     src/components/*.cxx
                     LINK_LIBRARIES CosmicGeneratorLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.txt share/*.py )

