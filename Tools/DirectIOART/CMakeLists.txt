################################################################################
# Package: DirectIOART
################################################################################

# Declare the package name:
atlas_subdir( DirectIOART  )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          TestPolicy )

# Install files from the package:
atlas_install_scripts( test/*.sh test/*.py )

# Install joboptions
atlas_install_joboptions( share/*.py )
