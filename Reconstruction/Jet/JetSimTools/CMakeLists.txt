# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( JetSimTools )

# Component(s) in the package:
atlas_add_component( JetSimTools
                     Root/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AsgTools xAODTruth JetUtils )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

