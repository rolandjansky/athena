################################################################################
# Package: TrkAlignableSurfaces
################################################################################

# Declare the package name:
atlas_subdir( TrkAlignableSurfaces )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          DetectorDescription/GeoPrimitives
                          DetectorDescription/Identifier
                          Tracking/TrkDetDescr/TrkSurfaces )

# External dependencies:
find_package( Eigen )

# Component(s) in the package:
atlas_add_library( TrkAlignableSurfaces
                   src/*.cxx
                   PUBLIC_HEADERS TrkAlignableSurfaces
                   INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS}
                   LINK_LIBRARIES ${EIGEN_LIBRARIES} GeoPrimitives Identifier TrkSurfaces )

