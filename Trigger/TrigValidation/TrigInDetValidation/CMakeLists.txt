# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigInDetValidation )

# External dependencies:
find_package( requests )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/*.py)
atlas_install_data( share/*.json )
atlas_install_runtime( share/*.py TIDAbuild )

atlas_install_scripts( scripts/TIDA*.py test/test*.py POST_BUILD_CMD ${ATLAS_FLAKE8} --extend-ignore=ATL902 )

# Unit test for python test scripts:
atlas_add_test( TrigValSteeringUT
                SCRIPT trigvalsteering-unit-tester.py ${CMAKE_CURRENT_SOURCE_DIR}/test
                PROPERTIES TIMEOUT 600
                POST_EXEC_SCRIPT nopost.sh )
