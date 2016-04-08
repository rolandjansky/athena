################################################################################
# Package: TrkVertexKinematicFitterUtils
################################################################################

# Declare the package name:
atlas_subdir( TrkVertexKinematicFitterUtils )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Tracking/TrkVertexFitter/TrkVertexFitterInterfaces )

# Component(s) in the package:
atlas_add_library( TrkVertexKinematicFitterUtils
                   src/*.cxx
                   PUBLIC_HEADERS TrkVertexKinematicFitterUtils
                   LINK_LIBRARIES TrkVertexFitterInterfaces )

