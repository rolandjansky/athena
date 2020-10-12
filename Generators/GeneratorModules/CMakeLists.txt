# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( GeneratorModules )

# External dependencies:
find_package( CLHEP )
find_package( HepPDT )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( GeneratorModulesLib
                   src/*.cxx
                   PUBLIC_HEADERS GeneratorModules
                   INCLUDE_DIRS ${HEPPDT_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} 
                   DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${HEPPDT_LIBRARIES} ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} AtlasHepMCLib AthenaBaseComps AthenaKernel GaudiKernel GeneratorObjects StoreGateLib )

atlas_add_component( GeneratorModules
                     src/components/*.cxx
                     LINK_LIBRARIES GeneratorModulesLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )

