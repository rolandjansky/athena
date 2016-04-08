################################################################################
# Package: GeoPrimitives
################################################################################

# Declare the package name:
atlas_subdir( GeoPrimitives )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/CxxUtils
                          Event/EventPrimitives )

# External dependencies:
find_package( CLHEP )
find_package( Eigen )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( GeoPrimitives
                   PUBLIC_HEADERS GeoPrimitives
                   INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                   DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${CLHEP_LIBRARIES} ${EIGEN_LIBRARIES} CxxUtils EventPrimitives )

atlas_add_dictionary( GeoPrimitivesDict
                      GeoPrimitives/GeoPrimitivesDict.h
                      GeoPrimitives/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} ${EIGEN_LIBRARIES} CxxUtils EventPrimitives GeoPrimitives )

