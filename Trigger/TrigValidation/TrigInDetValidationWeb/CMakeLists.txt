################################################################################
# Package: TrigInDetValidationWeb
################################################################################

# Declare the package name:
atlas_subdir( TrigInDetValidationWeb )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          TestPolicy )

# Install files from the package:
atlas_install_runtime( share/TIDAWeb )

