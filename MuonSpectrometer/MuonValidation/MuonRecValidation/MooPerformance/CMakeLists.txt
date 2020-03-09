################################################################################
# Package: MooPerformance
################################################################################

# Declare the package name:
atlas_subdir( MooPerformance )

# Install files from the package:
atlas_install_joboptions( share/*.py )
atlas_install_runtime( test/MooPerformance_TestConfiguration.xml share/*.C share/*.h share/*.py scripts/*.py ExtraFiles/*.html )

