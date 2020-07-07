# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( MultiInputExample )

# Component(s) in the package:
atlas_add_component( MultiInputExample
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps AthenaPoolExampleData EventInfo GaudiKernel PileUpToolsLib )

# Install files from the package:
atlas_install_joboptions( share/MultiInputInit.py )
