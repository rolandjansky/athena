# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( ForwardTransport )

# External dependencies:
find_package( Geant4 )

# Component(s) in the package:
atlas_add_component( ForwardTransport
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${GEANT4_INCLUDE_DIRS}
                     LINK_LIBRARIES ${GEANT4_LIBRARIES} AtlasHepMCLib ForwardTracker GaudiKernel G4AtlasToolsLib ForwardTransportSvcLib )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/*.py )

