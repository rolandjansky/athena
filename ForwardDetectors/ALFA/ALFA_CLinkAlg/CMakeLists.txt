# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( ALFA_CLinkAlg )

# Component(s) in the package:
atlas_add_component( ALFA_CLinkAlg
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps AthenaKernel AthLinks xAODForward ALFA_CLinkEv GaudiKernel EventInfo )

# Install files from the package:
atlas_install_joboptions( share/*.py )
