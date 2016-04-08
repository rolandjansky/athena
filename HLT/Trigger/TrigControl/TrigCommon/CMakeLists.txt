################################################################################
# Package: TrigCommon
################################################################################

# Declare the package name:
atlas_subdir( TrigCommon )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

