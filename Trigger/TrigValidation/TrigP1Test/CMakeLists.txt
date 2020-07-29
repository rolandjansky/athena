# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigP1Test )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_scripts( bin/*.py test/test*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_scripts( bin/*.sh test/test*.sh )
atlas_install_joboptions( share/*.py )
atlas_install_data( share/*.ref )
atlas_install_runtime( Testing/*.trans Testing/*.conf )

# Unit tests:
atlas_add_test( TrigValSteeringUT
                SCRIPT trigvalsteering-unit-tester.py ${CMAKE_CURRENT_SOURCE_DIR}/test
                PROPERTIES TIMEOUT 300
                POST_EXEC_SCRIPT nopost.sh )
