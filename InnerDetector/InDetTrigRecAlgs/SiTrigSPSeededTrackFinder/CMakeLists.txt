################################################################################
# Package: SiTrigSPSeededTrackFinder
################################################################################

# Declare the package name:
atlas_subdir( SiTrigSPSeededTrackFinder )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          GaudiKernel
                          Trigger/TrigSteer/TrigInterfaces
                          PRIVATE
                          Control/AthenaKernel
                          DetectorDescription/GeoPrimitives
                          DetectorDescription/IRegionSelector
                          Event/EventPrimitives
                          InnerDetector/InDetRecTools/InDetRecToolInterfaces
                          Tracking/TrkEvent/TrkMeasurementBase
                          Tracking/TrkEvent/TrkParameters
                          Tracking/TrkEvent/TrkRIO_OnTrack
                          Tracking/TrkEvent/TrkTrack
                          Trigger/TrigEvent/TrigInDetEvent
                          Trigger/TrigEvent/TrigParticle
                          Trigger/TrigEvent/TrigSteeringEvent
                          Trigger/TrigTools/TrigTimeAlgs )

# External dependencies:
find_package( Eigen )

# Component(s) in the package:
atlas_add_component( SiTrigSPSeededTrackFinder
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS}
                     LINK_LIBRARIES ${EIGEN_LIBRARIES} GaudiKernel TrigInterfacesLib AthenaKernel GeoPrimitives IRegionSelector EventPrimitives InDetRecToolInterfaces TrkMeasurementBase TrkParameters TrkRIO_OnTrack TrkTrack TrigInDetEvent TrigParticle TrigSteeringEvent TrigTimeAlgsLib )

# Install files from the package:
atlas_install_headers( SiTrigSPSeededTrackFinder )
atlas_install_python_modules( python/*.py )

