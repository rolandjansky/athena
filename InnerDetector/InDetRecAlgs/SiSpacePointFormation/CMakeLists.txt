################################################################################
# Package: SiSpacePointFormation
################################################################################

# Declare the package name:
atlas_subdir( SiSpacePointFormation )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/AthContainers
                          DetectorDescription/GeoPrimitives
                          DetectorDescription/Identifier
                          GaudiKernel
                          InnerDetector/InDetDetDescr/InDetReadoutGeometry
                          InnerDetector/InDetRecEvent/InDetPrepRawData
                          InnerDetector/InDetRecTools/SiSpacePointTool
                          Tracking/TrkEvent/TrkSpacePoint
                          Tracking/TrkEvent/VxVertex
                          PRIVATE
                          Control/CxxUtils
                          DetectorDescription/AtlasDetDescr
                          Event/xAOD/xAODEventInfo
                          InnerDetector/InDetConditions/InDetBeamSpotService
                          InnerDetector/InDetDetDescr/InDetIdentifier )

# External dependencies:
find_package( Eigen )

# Component(s) in the package:
atlas_add_library( SiSpacePointFormationLib
                   src/*.cxx
                   PUBLIC_HEADERS SiSpacePointFormation
                   INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS}
                   LINK_LIBRARIES ${EIGEN_LIBRARIES} AthenaBaseComps AthContainers GeoPrimitives Identifier GaudiKernel InDetReadoutGeometry InDetPrepRawData TrkSpacePoint VxVertex SiSpacePointToolLib
                   PRIVATE_LINK_LIBRARIES CxxUtils AtlasDetDescr xAODEventInfo InDetIdentifier )

atlas_add_component( SiSpacePointFormation
                     src/components/*.cxx
                     INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS}
                     LINK_LIBRARIES ${EIGEN_LIBRARIES} AthenaBaseComps AthContainers GeoPrimitives Identifier GaudiKernel InDetReadoutGeometry InDetPrepRawData SiSpacePointToolLib TrkSpacePoint VxVertex CxxUtils AtlasDetDescr xAODEventInfo InDetIdentifier SiSpacePointFormationLib )

