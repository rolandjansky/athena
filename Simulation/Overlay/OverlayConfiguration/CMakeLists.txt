################################################################################
# Package: OverlayConfiguration
################################################################################

# Declare the package name:
atlas_subdir( OverlayConfiguration )

# External dependencies:

# Install files from the package:
atlas_install_python_modules( python/*.py
                              POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_runtime( test/OverlayTest.py )

# Setup and run tests
atlas_add_test( OverlayTest_MC
                SCRIPT test/OverlayTest.py
                PROPERTIES TIMEOUT 300
                POST_EXEC_SCRIPT nopost.sh )

atlas_add_test( OverlayTest_data
                SCRIPT test/OverlayTest.py -d
                PROPERTIES TIMEOUT 300
                POST_EXEC_SCRIPT nopost.sh )
