################################################################################
# Package: CaloDigEx
################################################################################

# Declare the package name:
atlas_subdir( CaloDigEx )

# Install files from the package:
atlas_install_joboptions( share/*.py )
atlas_install_runtime( test/CaloDigEx_TestConfiguration.xml )

