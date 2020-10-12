# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TruthIO )

# External dependencies:
find_package( HepPDT )

# Component(s) in the package:
atlas_add_component( TruthIO
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${HEPPDT_INCLUDE_DIRS} 
                     LINK_LIBRARIES ${HEPPDT_LIBRARIES} AtlasHepMCLib AtlasHepMCfioLib AthenaBaseComps GaudiKernel GeneratorModulesLib StoreGateLib EventInfo GeneratorObjects )

# Install files from the package:
atlas_install_joboptions( share/*.py )
