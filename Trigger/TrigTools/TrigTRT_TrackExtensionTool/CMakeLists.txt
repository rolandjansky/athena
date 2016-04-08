################################################################################
# Package: TrigTRT_TrackExtensionTool
################################################################################

# Declare the package name:
atlas_subdir( TrigTRT_TrackExtensionTool )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          GaudiKernel
                          InnerDetector/InDetDetDescr/InDetIdentifier
                          InnerDetector/InDetDetDescr/InDetReadoutGeometry
                          InnerDetector/InDetRecEvent/InDetPrepRawData
                          Tracking/TrkExtrapolation/TrkExInterfaces
                          Trigger/TrigEvent/TrigInDetEvent
                          Trigger/TrigTools/TrigInDetToolInterfaces
                          Trigger/TrigTools/TrigInDetTrackFitter
                          PRIVATE
                          DetectorDescription/GeoPrimitives
                          DetectorDescription/IRegionSelector
                          DetectorDescription/Identifier
                          Event/ByteStreamCnvSvcBase
                          MagneticField/MagFieldInterfaces
                          Tracking/TrkDetDescr/TrkSurfaces
                          Tracking/TrkEvent/TrkParameters
                          Tracking/TrkEvent/TrkRIO_OnTrack
                          Tracking/TrkFitter/TrkDistributedKalmanFilter
                          Tracking/TrkTools/TrkToolInterfaces
                          Trigger/TrigEvent/TrigSteeringEvent
                          Trigger/TrigTools/TrigTimeAlgs )

# External dependencies:
find_package( Eigen )

# Component(s) in the package:
atlas_add_component( TrigTRT_TrackExtensionTool
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS}
                     LINK_LIBRARIES ${EIGEN_LIBRARIES} AthenaBaseComps GaudiKernel InDetIdentifier InDetReadoutGeometry InDetPrepRawData TrkExInterfaces TrigInDetEvent GeoPrimitives IRegionSelector Identifier ByteStreamCnvSvcBaseLib MagFieldInterfaces TrkSurfaces TrkParameters TrkRIO_OnTrack TrkDistributedKalmanFilterLib TrkToolInterfaces TrigSteeringEvent TrigTimeAlgsLib )

# Install files from the package:
atlas_install_headers( TrigTRT_TrackExtensionTool )
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

