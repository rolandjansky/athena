################################################################################
# Package: TrigInDetValidationReference
################################################################################

# Declare the package name:
atlas_subdir( TrigInDetValidationReference )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          TestPolicy )

# Install files from the package:
atlas_install_runtime( share/data-*.root share/expert/expert*.root )

