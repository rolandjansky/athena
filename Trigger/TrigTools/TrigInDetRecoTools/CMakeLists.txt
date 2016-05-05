################################################################################
# Package: TrigInDetRecoTools
################################################################################

# Declare the package name:
atlas_subdir( TrigInDetRecoTools )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          GaudiKernel
                          InnerDetector/InDetRecEvent/InDetPrepRawData
                          InnerDetector/InDetRecEvent/SiSpacePoint
                          InnerDetector/InDetRecTools/InDetRecToolInterfaces
                          Tracking/TrkEvent/TrkSpacePoint
                          Tracking/TrkEvent/TrkTrack
                          Trigger/TrigEvent/TrigInDetEvent
                          Trigger/TrigEvent/TrigInDetPattRecoEvent
                          Trigger/TrigEvent/TrigSteeringEvent
                          Trigger/TrigTools/TrigInDetToolInterfaces
                          PRIVATE
                          Control/AthenaKernel
                          DetectorDescription/GeoPrimitives
                          DetectorDescription/IRegionSelector
                          Event/ByteStreamCnvSvcBase
                          Event/EventInfo
                          InnerDetector/InDetConditions/InDetBeamSpotService
                          InnerDetector/InDetDetDescr/InDetIdentifier
                          InnerDetector/InDetDetDescr/InDetReadoutGeometry
                          InnerDetector/InDetRecEvent/InDetRIO_OnTrack
                          Tracking/TrkDetDescr/TrkSurfaces
                          Tracking/TrkEvent/TrkEventPrimitives
                          Tracking/TrkEvent/TrkParameters
                          Tracking/TrkEvent/TrkRIO_OnTrack
                          Tracking/TrkEvent/TrkSegment
                          Trigger/TrigTools/TrigTimeAlgs )

# External dependencies:
find_package( Eigen )
find_package( tdaq-common )

# Component(s) in the package:
atlas_add_component( TrigInDetRecoTools
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${TDAQ-COMMON_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                     LINK_LIBRARIES ${TDAQ-COMMON_LIBRARIES} ${EIGEN_LIBRARIES} AthenaBaseComps GaudiKernel InDetPrepRawData SiSpacePoint InDetRecToolInterfaces TrkSpacePoint TrkTrack TrigInDetEvent TrigInDetPattRecoEvent TrigSteeringEvent AthenaKernel GeoPrimitives IRegionSelector ByteStreamCnvSvcBaseLib EventInfo InDetIdentifier InDetReadoutGeometry InDetRIO_OnTrack TrkSurfaces TrkEventPrimitives TrkParameters TrkRIO_OnTrack TrkSegment TrigTimeAlgsLib )

# Install files from the package:
atlas_install_headers( TrigInDetRecoTools )
atlas_install_python_modules( python/*.py )

