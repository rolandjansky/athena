################################################################################
# Package: TBDetDescr
################################################################################

# Declare the package name:
atlas_subdir( TBDetDescr )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaKernel
                          Control/AthContainers
                          DetectorDescription/GeoPrimitives
                          PRIVATE
                          GaudiKernel )

# External dependencies:
find_package( Eigen )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( TBDetDescr
                   src/TB*.cxx
                   PUBLIC_HEADERS TBDetDescr
                   INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${EIGEN_LIBRARIES} AthContainers GeoPrimitives
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} GaudiKernel )

atlas_add_dictionary( TBDetDescrDict
                      TBDetDescr/TBDetDescrDict.h
                      TBDetDescr/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} ${EIGEN_LIBRARIES} AthContainers GeoPrimitives GaudiKernel TBDetDescr )

