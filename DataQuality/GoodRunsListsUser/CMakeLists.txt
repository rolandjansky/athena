# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( GoodRunsListsUser )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO )

# Component(s) in the package:
atlas_add_component( GoodRunsListsUser
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaBaseComps GoodRunsListsLib EventInfo GaudiKernel TrigDecisionToolLib TrigDecisionEvent )

# Install files from the package:
atlas_install_joboptions( share/*.py )
