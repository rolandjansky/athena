################################################################################
# Package: MuonAlignExample
################################################################################

# Declare the package name:
atlas_subdir( MuonAlignExample )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )
#atlas_install_runtime( test/MuonAlignExample_TestConfiguration.xml )

