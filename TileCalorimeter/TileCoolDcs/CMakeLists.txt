################################################################################
# Package: TileCoolDcs
################################################################################

# Declare the package name:
atlas_subdir( TileCoolDcs )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_scripts( share/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} --extend-ignore=F401,F821 )
atlas_install_runtime( share/*.dat )
