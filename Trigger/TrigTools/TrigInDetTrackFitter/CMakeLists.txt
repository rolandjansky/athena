################################################################################
# Package: TrigInDetTrackFitter
################################################################################

# Declare the package name:
atlas_subdir( TrigInDetTrackFitter )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          GaudiKernel
                          InnerDetector/InDetDetDescr/InDetIdentifier
                          InnerDetector/InDetDetDescr/InDetReadoutGeometry
                          MagneticField/MagFieldInterfaces
                          Tracking/TrkEvent/TrkTrack
                          Tracking/TrkExtrapolation/TrkExInterfaces
                          Tracking/TrkFitter/TrkDistributedKalmanFilter
                          Tracking/TrkFitter/TrkFitterInterfaces
                          Tracking/TrkFitter/TrkFitterUtils
                          Tracking/TrkTools/TrkToolInterfaces
                          Trigger/TrigEvent/TrigInDetEvent
                          Trigger/TrigTools/TrigInDetToolInterfaces
                          PRIVATE
                          DetectorDescription/AtlasDetDescr
                          InnerDetector/InDetRecEvent/InDetPrepRawData
                          InnerDetector/InDetRecEvent/InDetRIO_OnTrack
                          Tracking/TrkDetDescr/TrkSurfaces
                          Tracking/TrkEvent/TrkEventPrimitives
                          Tracking/TrkEvent/TrkParameters
                          Tracking/TrkEvent/TrkPrepRawData
                          Tracking/TrkEvent/TrkRIO_OnTrack
                          Trigger/TrigTools/TrigTimeAlgs )

# Component(s) in the package:
atlas_add_component( TrigInDetTrackFitter
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps GaudiKernel InDetIdentifier InDetReadoutGeometry MagFieldInterfaces TrkTrack TrkExInterfaces TrkDistributedKalmanFilterLib TrkFitterInterfaces TrkFitterUtils TrkToolInterfaces TrigInDetEvent AtlasDetDescr InDetPrepRawData InDetRIO_OnTrack TrkSurfaces TrkEventPrimitives TrkParameters TrkPrepRawData TrkRIO_OnTrack TrigTimeAlgsLib )

# Install files from the package:
atlas_install_headers( TrigInDetTrackFitter )
atlas_install_python_modules( python/*.py )

