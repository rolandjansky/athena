# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( DerivationFrameworkFlavourTag )

# External dependencies:
find_package( ROOT COMPONENTS Core RIO )

# Component(s) in the package:
atlas_add_component( DerivationFrameworkFlavourTag
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps FlavorTagDiscriminants GaudiKernel xAODJet xAODTracking )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

# add the test utility
atlas_add_executable( test_btagging
                      util/test-btagging.cxx
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} xAODJet xAODRootAccess )

atlas_add_executable( test_jet_links
                      util/test-jet-links.cxx
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} xAODJet xAODRootAccess )
