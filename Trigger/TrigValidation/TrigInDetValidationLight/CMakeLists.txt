################################################################################
# Package: TrigInDetValidationLight
################################################################################
 
# Declare the package name:
atlas_subdir( TrigInDetValidationLight )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          TestPolicy )

# Install files from the package:
atlas_install_runtime( test/TrigInDetValidationLight_TestConfiguration.xml )

