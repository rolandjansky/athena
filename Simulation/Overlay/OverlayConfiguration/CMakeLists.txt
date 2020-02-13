################################################################################
# Package: OverlayConfiguration
################################################################################

# Declare the package name:
atlas_subdir( OverlayConfiguration )

# External dependencies:

# Install files from the package:
atlas_install_python_modules( python/*.py
                              POST_BUILD_CMD ${ATLAS_FLAKE8} )

# Setup and run tests
atlas_add_test( OverlayTest_MC
                SCRIPT test/OverlayTest_MC.py
                PROPERTIES TIMEOUT 300 )

atlas_add_test( OverlayTest_MC_MT
                SCRIPT test/OverlayTest_MC.py -n 25 -t 3
                PROPERTIES TIMEOUT 300 )

# Data tests disabled waiting for upstream changes
# atlas_add_test( OverlayTest_data
#                 SCRIPT test/OverlayTest_data.py
#                 PROPERTIES TIMEOUT 300 )
#
# atlas_add_test( OverlayTest_data_MT
#                 SCRIPT test/OverlayTest_data.py -n 10 -t 3
#                 PROPERTIES TIMEOUT 300 )
