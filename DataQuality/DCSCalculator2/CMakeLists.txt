################################################################################
# Package: DCSCalculator2
################################################################################

# Declare the package name:
atlas_subdir( DCSCalculator2 )

# Install files from the package:
atlas_install_python_modules( python/*.py python/subdetectors )
atlas_install_scripts( share/*.py )

