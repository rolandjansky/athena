################################################################################
# Package: DerivationFrameworkPhysicsValidation
################################################################################

# Declare the package name:
atlas_subdir( DerivationFrameworkPhysicsValidation  )

# Install files from the package:
atlas_install_joboptions( share/*.py )

# Python modules
atlas_install_python_modules( python/*.py )
