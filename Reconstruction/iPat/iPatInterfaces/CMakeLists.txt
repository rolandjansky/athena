################################################################################
# Package: iPatInterfaces
################################################################################

# Declare the package name:
atlas_subdir( iPatInterfaces )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaKernel
                          DetectorDescription/GeoPrimitives
                          GaudiKernel
                          InnerDetector/InDetRecEvent/InDetPrepRawData
                          Reconstruction/iPat/iPatTrack
                          Tracking/TrkEvent/TrkParameters )

# External dependencies:
find_package( Eigen )

# Component(s) in the package:
atlas_add_library( iPatInterfaces
                   PUBLIC_HEADERS iPatInterfaces
                   INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS}
                   LINK_LIBRARIES ${EIGEN_LIBRARIES} AthenaKernel GeoPrimitives GaudiKernel InDetPrepRawData iPatTrack TrkParameters )

