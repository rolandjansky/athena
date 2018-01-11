################################################################################
# Package: POOLCore
################################################################################

# Declare the package name:
atlas_subdir( POOLCore )

# Component(s) in the package:
atlas_add_library( POOLCore
                   src/*.cpp
                   PUBLIC_HEADERS POOLCore )

