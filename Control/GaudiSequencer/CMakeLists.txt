# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( GaudiSequencer )

# External dependencies:
find_package( valgrind )

# Component(s) in the package:
atlas_add_component( GaudiSequencer
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${VALGRIND_INCLUDE_DIRS}
                     LINK_LIBRARIES ${VALGRIND_LIBRARIES} AthenaBaseComps AthenaKernel CxxUtils GaudiKernel )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/*.py )

