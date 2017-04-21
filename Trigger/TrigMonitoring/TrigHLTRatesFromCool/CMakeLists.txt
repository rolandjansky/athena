################################################################################
# Package: TrigHLTRatesFromCool
################################################################################

# Declare the package name:
atlas_subdir( TrigHLTRatesFromCool )

# Install files from the package:
atlas_install_scripts( bin/*.py )
atlas_install_python_modules( python/*.py )
