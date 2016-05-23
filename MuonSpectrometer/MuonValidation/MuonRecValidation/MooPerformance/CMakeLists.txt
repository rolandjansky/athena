################################################################################
# Package: MooPerformance
################################################################################

# Declare the package name:
atlas_subdir( MooPerformance )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          TestPolicy )

# Install files from the package:
atlas_install_joboptions( share/*.py )
atlas_install_runtime( test/MooPerformance_TestConfiguration.xml share/*.C share/*.h share/*.py scripts/*.py ExtraFiles/*.html )

