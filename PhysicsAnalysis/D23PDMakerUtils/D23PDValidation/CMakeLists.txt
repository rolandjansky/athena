################################################################################
# Package: D23PDValidation
################################################################################

# Declare the package name:
atlas_subdir( D23PDValidation )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          TestPolicy )

# Install files from the package:
atlas_install_runtime( test/D23PDValidation_TestConfiguration.xml )

