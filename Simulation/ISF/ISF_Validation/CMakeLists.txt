################################################################################
# Package: ISF_Validation
################################################################################

# Declare the package name:
atlas_subdir( ISF_Validation )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          TestPolicy )

# Install files from the package:
atlas_install_runtime( test/ISF_Validation_TestConfiguration.xml )
atlas_install_scripts( scripts/*.sh test/*.sh )
