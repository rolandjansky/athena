################################################################################
# Package: TrkDetElementBase
################################################################################

# Declare the package name:
atlas_subdir( TrkDetElementBase )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          DetectorDescription/GeoModel/GeoModelKernel
                          DetectorDescription/GeoPrimitives
                          DetectorDescription/Identifier
                          Event/EventPrimitives )

# External dependencies:
find_package( Eigen )

# Component(s) in the package:
atlas_add_library( TrkDetElementBase
                   src/*.cxx
                   PUBLIC_HEADERS TrkDetElementBase
                   INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS}
                   LINK_LIBRARIES ${EIGEN_LIBRARIES} GeoModelKernel GeoPrimitives Identifier EventPrimitives )

