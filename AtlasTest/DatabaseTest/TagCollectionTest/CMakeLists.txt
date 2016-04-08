################################################################################
# Package: TagCollectionTest
################################################################################

# Declare the package name:
atlas_subdir( TagCollectionTest )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/MinimalRunTime
                          TestPolicy
                          Tools/AtlasDoxygen )

# Install files from the package:
atlas_install_headers( TagCollectionTest )
atlas_install_joboptions( share/*.py )

