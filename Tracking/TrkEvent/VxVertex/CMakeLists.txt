################################################################################
# Package: VxVertex
################################################################################

# Declare the package name:
atlas_subdir( VxVertex )

# External dependencies:
find_package( Eigen )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( VxVertex
                   src/*.cxx
                   PUBLIC_HEADERS VxVertex
                   INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${EIGEN_LIBRARIES} AthLinks AthContainers GeoPrimitives 
                   EventPrimitives TrkEventPrimitives  TrkNeutralParameters 
                   TrkTrackLink TrkParameters 
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} GaudiKernel )

atlas_add_dictionary( VxVertexDict
                      VxVertex/VxVertexDict.h
                      VxVertex/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} ${EIGEN_LIBRARIES} AthLinks AthContainers GeoPrimitives 
                      EventPrimitives TrkEventPrimitives TrkNeutralParameters GaudiKernel TrkParameters VxVertex
                      ELEMENT_LINKS VxContainer )

