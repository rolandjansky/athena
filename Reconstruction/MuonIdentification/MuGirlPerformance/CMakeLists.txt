################################################################################
# Package: MuGirlPerformance
################################################################################

# Declare the package name:
atlas_subdir( MuGirlPerformance )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          TestPolicy )

# Install files from the package:
atlas_install_joboptions( share/*.py )
#atlas_install_runtime( test/MuGirlPerformance_TestConfiguration.xml share/*.C share/*.h )

