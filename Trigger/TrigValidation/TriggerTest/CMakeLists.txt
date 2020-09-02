# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TriggerTest )

# Install files from the package:
atlas_install_scripts( test/test*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_scripts( test/exec*.sh test/test*.sh test/test*.py )
atlas_install_joboptions( share/*.py )
atlas_install_runtime( share/*.conf )
atlas_install_data( share/*.ref )

# Unit tests:
atlas_add_test( TrigValSteeringUT
                SCRIPT trigvalsteering-unit-tester.py ${CMAKE_CURRENT_SOURCE_DIR}/test
                PROPERTIES TIMEOUT 300
                POST_EXEC_SCRIPT nopost.sh )
