################################################################################
# Package: TRT_TrigSeededTrackFinder
################################################################################

# Declare the package name:
atlas_subdir( TRT_TrigSeededTrackFinder )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          GaudiKernel
                          InnerDetector/InDetRecTools/InDetRecToolInterfaces
                          Tracking/TrkEvent/TrkSegment
                          Tracking/TrkFitter/TrkFitterInterfaces
                          Trigger/TrigSteer/TrigInterfaces
                          Trigger/TrigTools/TrigTimeAlgs
                          PRIVATE
                          DetectorDescription/IRegionSelector
                          Event/EventPrimitives
                          Tracking/TrkEvent/TrkEventPrimitives
                          Tracking/TrkEvent/TrkParameters
                          Tracking/TrkEvent/TrkPseudoMeasurementOnTrack
                          Tracking/TrkEvent/TrkTrack
                          Trigger/TrigEvent/TrigParticle
                          Trigger/TrigEvent/TrigSteeringEvent )

# Component(s) in the package:
atlas_add_component( TRT_TrigSeededTrackFinder
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES GaudiKernel InDetRecToolInterfaces TrkSegment TrkFitterInterfaces TrigInterfacesLib TrigTimeAlgsLib IRegionSelector EventPrimitives TrkEventPrimitives TrkParameters TrkPseudoMeasurementOnTrack TrkTrack TrigParticle TrigSteeringEvent )

# Install files from the package:
atlas_install_headers( TRT_TrigSeededTrackFinder )
atlas_install_python_modules( python/*.py )

