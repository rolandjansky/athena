################################################################################
# Package: CoWTools
################################################################################

# Declare the package name:
atlas_subdir( CoWTools )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          GaudiKernel )

# Component(s) in the package:
atlas_add_library( CoWTools
                   src/*.cxx
                   PUBLIC_HEADERS CoWTools
                   LINK_LIBRARIES GaudiKernel )

