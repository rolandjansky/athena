# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrackRecordGenerator )

# External dependencies:
find_package( CLHEP )

# Component(s) in the package:
atlas_add_component( TrackRecordGenerator
                     src/TrackRecordGenerator.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                     DEFINITIONS ${CLHEP_DEFINITIONS}
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} AtlasHepMCLib GeneratorModulesLib TrackRecordLib )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
