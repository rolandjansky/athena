# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( SimulationConfig )

# Install files from the package:
#atlas_install_joboptions( share/*.py
#                          POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_python_modules( python/*.py
                              POST_BUILD_CMD ${ATLAS_FLAKE8} )
#atlas_install_runtime( scripts/*.py )
#atlas_install_runtime( test/OverlayTest.py )

# Setup and run tests

# TODO: temporarily disabling this test, will be fixed in a separate MR
#atlas_add_test( OverlayTest_MC
#                SCRIPT test/OverlayTest.py
#                PROPERTIES TIMEOUT 900
#                POST_EXEC_SCRIPT nopost.sh )
