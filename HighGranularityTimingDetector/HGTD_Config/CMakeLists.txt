################################################################################
# Package: HGTD_Config
################################################################################

# Declare the package name:
atlas_subdir( HGTD_Config )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
