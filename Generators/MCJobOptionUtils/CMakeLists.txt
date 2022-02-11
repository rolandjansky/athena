################################################################################
# Package: MCJobOptionUtils
################################################################################

# Declare the package name:
atlas_subdir( MCJobOptionUtils )

# Install python modules from the package:
atlas_install_python_modules( python/*.py )

# Install files from the package:
atlas_install_joboptions( share/common/*.py )

