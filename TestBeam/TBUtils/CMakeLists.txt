################################################################################
# Package: TBUtils
################################################################################

# Declare the package name:
atlas_subdir( TBUtils )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          TestBeam/TBEvent )

# Install files from the package:
atlas_install_headers( TBUtils )

