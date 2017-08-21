################################################################################
# Package: DerivationFrameworkART
################################################################################

# Declare the package name:
atlas_subdir( DerivationFrameworkART  )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          TestPolicy )

# Install files from the package:
atlas_install_scripts( test/*.sh test/*.py )
