################################################################################
# Package: PmbCxxUtils
################################################################################

# Declare the package name:
atlas_subdir( PmbCxxUtils )

# Component(s) in the package:
atlas_add_library( PmbCxxUtils
                   src/*.cxx
                   PUBLIC_HEADERS PmbCxxUtils )

