################################################################################
# Package: TrkPrepRawData
################################################################################

# Declare the package name:
atlas_subdir( TrkPrepRawData )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaKernel
                          Control/AthContainers
                          Control/AthLinks
                          DetectorDescription/GeoPrimitives
                          DetectorDescription/Identifier
                          Event/EventContainers
                          Event/EventPrimitives
                          PRIVATE
                          GaudiKernel )

# External dependencies:
find_package( Eigen )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( TrkPrepRawData
                   src/*.cxx
                   PUBLIC_HEADERS TrkPrepRawData
                   INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${EIGEN_LIBRARIES} AthLinks AthContainers AthenaKernel GeoPrimitives Identifier EventPrimitives EventContainers
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} GaudiKernel )

