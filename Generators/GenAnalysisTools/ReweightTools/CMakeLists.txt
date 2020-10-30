# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( ReweightTools )

# External dependencies:
find_package( Lhapdf )

# Component(s) in the package:
atlas_add_library( ReweightToolsLib
                   src/*.cxx
                   PUBLIC_HEADERS ReweightTools
                   INCLUDE_DIRS ${LHAPDF_INCLUDE_DIRS} 
                   LINK_LIBRARIES ${LHAPDF_LIBRARIES} AtlasHepMCLib AthenaBaseComps GaudiKernel GenInterfacesLib
                   PRIVATE_LINK_LIBRARIES AthContainers GeneratorObjects )

atlas_add_component( ReweightTools
                     src/components/*.cxx
                     LINK_LIBRARIES ReweightToolsLib )

# Install files from the package:
atlas_install_joboptions( share/*.py )

