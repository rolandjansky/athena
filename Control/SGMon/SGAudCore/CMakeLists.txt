################################################################################
# Package: SGAudCore
################################################################################

# Declare the package name:
atlas_subdir( SGAudCore )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          GaudiKernel )

# Component(s) in the package:
atlas_add_library( SGAudCore
                   src/*.cxx
                   PUBLIC_HEADERS SGAudCore
                   LINK_LIBRARIES GaudiKernel )

