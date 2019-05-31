################################################################################
# Package: OverlayConfiguration
################################################################################

# Declare the package name:
atlas_subdir( OverlayConfiguration )

# External dependencies:

# Install files from the package:
atlas_install_python_modules( python/*.py )

# Check python syntax:
atlas_add_test( flake8
   SCRIPT flake8 --select=ATL,F,E7,E9,W6 ${CMAKE_CURRENT_SOURCE_DIR}/python
   POST_EXEC_SCRIPT nopost.sh )
