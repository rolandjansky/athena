################################################################################
# Package: iPatTrackParameters
################################################################################

# Declare the package name:
atlas_subdir( iPatTrackParameters )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          DetectorDescription/GeoPrimitives
                          Event/EventPrimitives
                          PRIVATE
                          Control/AthenaKernel )

# External dependencies:
find_package( Eigen )

# Component(s) in the package:
atlas_add_library( iPatTrackParameters
                   src/PerigeeParameters.cxx
                   src/ScattererParameters.cxx
                   PUBLIC_HEADERS iPatTrackParameters
                   INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS}
                   LINK_LIBRARIES ${EIGEN_LIBRARIES} GeoPrimitives EventPrimitives
                   PRIVATE_LINK_LIBRARIES AthenaKernel )

