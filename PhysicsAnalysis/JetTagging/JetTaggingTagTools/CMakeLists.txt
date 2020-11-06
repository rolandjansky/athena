# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( JetTaggingTagTools )

# External dependencies:
find_package( CLHEP )

# Component(s) in the package:
atlas_add_library( JetTaggingTagToolsLib
                   src/*.cxx
                   PUBLIC_HEADERS JetTaggingTagTools
                   PRIVATE_INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                   PRIVATE_DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES AthenaBaseComps AthenaPoolUtilities TagEvent StoreGateLib
                   PRIVATE_LINK_LIBRARIES ${CLHEP_LIBRARIES} GaudiKernel JetEvent )

atlas_add_component( JetTaggingTagTools
                     src/components/*.cxx
                     LINK_LIBRARIES JetTaggingTagToolsLib )

# Install files from the package:
atlas_install_joboptions( share/*.py )

