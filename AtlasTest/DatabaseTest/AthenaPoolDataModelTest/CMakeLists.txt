################################################################################
# Package: AthenaPoolDataModelTest
################################################################################

# Declare the package name:
atlas_subdir( AthenaPoolDataModelTest )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/MinimalRunTime
                          TestPolicy )

# Install files from the package:
atlas_install_joboptions( share/*.py )

