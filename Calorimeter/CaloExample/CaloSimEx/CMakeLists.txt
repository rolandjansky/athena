################################################################################
# Package: CaloSimEx
################################################################################

# Declare the package name:
atlas_subdir( CaloSimEx )

# Install files from the package:
atlas_install_joboptions( share/*.py )
atlas_install_runtime( test/CaloSimEx_TestConfiguration.xml )

