################################################################################
# Package: TrigEDMConfig
################################################################################

# Declare the package name:
atlas_subdir( TrigEDMConfig )

# Install files from the package:
atlas_install_python_modules( python/*.py )

atlas_add_test( UnitTest SCRIPT python/testEDM.py )
