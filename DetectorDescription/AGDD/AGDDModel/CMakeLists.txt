################################################################################
# Package: AGDDModel
################################################################################

# Declare the package name:
atlas_subdir( AGDDModel )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          DetectorDescription/AGDD/AGDDKernel )

# Component(s) in the package:
atlas_add_library( AGDDModel
                   src/*.cxx
                   PUBLIC_HEADERS AGDDModel
                   LINK_LIBRARIES AGDDKernel )

