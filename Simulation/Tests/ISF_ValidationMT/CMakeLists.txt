################################################################################
# Package: ISF_ValidationMT
################################################################################

# Declare the package name:
atlas_subdir( ISF_ValidationMT )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          TestPolicy )

# Install files from the package:
atlas_install_scripts( test/*.sh )
