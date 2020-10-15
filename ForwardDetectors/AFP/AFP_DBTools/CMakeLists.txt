# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( AFP_DBTools )

# External dependencies:
find_package( Boost )

# Component(s) in the package:
atlas_add_component( AFP_DBTools
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES ${Boost_LIBRARIES} AthenaBaseComps AthenaKernel AthenaPoolUtilities )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )

# Test(s) in the package:
atlas_add_test( flake8_scripts
                SCRIPT ${ATLAS_FLAKE8} ${CMAKE_CURRENT_SOURCE_DIR}/scripts
                PROPERTIES TIMEOUT 300
                POST_EXEC_SCRIPT nopost.sh )
