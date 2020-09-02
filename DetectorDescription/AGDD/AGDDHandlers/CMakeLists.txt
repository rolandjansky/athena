################################################################################
# Package: AGDDHandlers
################################################################################

# Declare the package name:
atlas_subdir( AGDDHandlers )

# Component(s) in the package:
atlas_add_library( AGDDHandlers
                   src/*.cxx
                   PUBLIC_HEADERS AGDDHandlers
                   LINK_LIBRARIES AGDDControl AGDDKernel
                   PRIVATE_LINK_LIBRARIES AGDDModel )

