################################################################################
# Package: Tier0ChainTests
################################################################################

# Declare the package name:
atlas_subdir( Tier0ChainTests )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_scripts( scripts/tct_*.py scripts/tct_*.sh )
atlas_install_runtime( test/Tier0ChainTests_TestConfiguration.xml )
atlas_install_scripts( test/*.sh test/*.py )


