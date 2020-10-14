# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( OjfJet )

# External dependencies:
find_package( CLHEP )

# Component(s) in the package:
atlas_add_library( OjfJet
                   src/*.cxx
                   PUBLIC_HEADERS OjfJet
                   INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                   DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${CLHEP_LIBRARIES} AthenaKernel )

