################################################################################
# Package: TileCoolDcs
################################################################################

# Declare the package name:
atlas_subdir( TileCoolDcs )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_scripts( share/*.py )
atlas_install_runtime( share/*.dat )

