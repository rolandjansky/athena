################################################################################
# Package: TrkValidationUtils
################################################################################

# Declare the package name:
atlas_subdir( TrkValidationUtils )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          DetectorDescription/GeoPrimitives
                          Event/EventPrimitives
                          PRIVATE
                          Tracking/TrkDetDescr/TrkSurfaces )

# External dependencies:
find_package( CLHEP )
find_package( Eigen )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( TrkValidationUtils
                   src/*.cxx
                   PUBLIC_HEADERS TrkValidationUtils
                   INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${CLHEP_LIBRARIES} ${EIGEN_LIBRARIES} GeoPrimitives EventPrimitives
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} TrkSurfaces )

