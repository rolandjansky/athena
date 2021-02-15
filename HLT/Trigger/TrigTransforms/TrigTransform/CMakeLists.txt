################################################################################
# Package: TrigTransform
################################################################################

# Declare the package name:
atlas_subdir( TrigTransform )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_runtime( scripts/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
