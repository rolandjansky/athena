# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( BoostAfterburner )

# External dependencies:
find_package( CLHEP )

# Component(s) in the package:
atlas_add_library( BoostAfterburnerLib
                   src/*.cxx
                   PUBLIC_HEADERS BoostAfterburner
                   PRIVATE_INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                   PRIVATE_DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES AthenaBaseComps
                   PRIVATE_LINK_LIBRARIES ${CLHEP_LIBRARIES} GeneratorObjects )

atlas_add_component( BoostAfterburner
                     src/components/*.cxx
                     LINK_LIBRARIES BoostAfterburnerLib )

# Install files from the package:
atlas_install_joboptions( share/*.py )

