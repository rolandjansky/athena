################################################################################
# Package: TrigHLTRatesFromCool
################################################################################

# Declare the package name:
atlas_subdir( TrigHLTRatesFromCool )

# Install files from the package:
atlas_install_scripts( bin/*.py )
atlas_install_python_modules( python/*.py )
atlas_add_test(pt_TrigHltRates  SCRIPT test/pt_TrigHltRates.py)
