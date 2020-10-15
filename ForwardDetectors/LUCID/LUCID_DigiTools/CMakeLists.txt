# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( LUCID_DigiTools )

# External dependencies:
find_package( CLHEP )

# Component(s) in the package:
atlas_add_library( LUCID_DigiTools
                   src/*.cxx
                   PUBLIC_HEADERS LUCID_DigiTools
                   INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                   DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${CLHEP_LIBRARIES}
                   PRIVATE_LINK_LIBRARIES GaudiKernel )

