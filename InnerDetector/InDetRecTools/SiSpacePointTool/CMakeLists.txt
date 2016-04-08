################################################################################
# Package: SiSpacePointTool
################################################################################

# Declare the package name:
atlas_subdir( SiSpacePointTool )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          DetectorDescription/GeoPrimitives
                          InnerDetector/InDetRecEvent/InDetPrepRawData
                          PRIVATE
                          GaudiKernel
                          InnerDetector/InDetDetDescr/InDetIdentifier
                          InnerDetector/InDetDetDescr/InDetReadoutGeometry
                          InnerDetector/InDetRecEvent/SiSpacePoint
                          Tracking/TrkEvent/TrkSpacePoint )

# External dependencies:
find_package( Eigen )

# Component(s) in the package:
atlas_add_library( SiSpacePointToolLib
                   src/*.cxx
                   PUBLIC_HEADERS SiSpacePointTool
                   INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS}
                   LINK_LIBRARIES ${EIGEN_LIBRARIES} AthenaBaseComps GeoPrimitives InDetPrepRawData
                   PRIVATE_LINK_LIBRARIES GaudiKernel InDetIdentifier InDetReadoutGeometry SiSpacePoint TrkSpacePoint )

atlas_add_component( SiSpacePointTool
                     src/components/*.cxx
                     INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS}
                     LINK_LIBRARIES ${EIGEN_LIBRARIES} AthenaBaseComps GeoPrimitives InDetPrepRawData GaudiKernel InDetIdentifier InDetReadoutGeometry SiSpacePoint TrkSpacePoint SiSpacePointToolLib )

