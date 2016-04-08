################################################################################
# Package: D3PDMakerConfig
################################################################################

# Declare the package name:
atlas_subdir( D3PDMakerConfig )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )
atlas_install_runtime( test/D3PDMakerConfig_TestConfiguration.xml )
atlas_install_scripts( scripts/D3PDSizeSummary.py )

