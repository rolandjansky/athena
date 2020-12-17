################################################################################
# Package: ISF_Example
################################################################################

# Declare the package name:
atlas_subdir( ISF_Example )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
