################################################################################
# Package: TRT_TrigTrackSegmentsFinder
################################################################################

# Declare the package name:
atlas_subdir( TRT_TrigTrackSegmentsFinder )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          GaudiKernel
                          InnerDetector/InDetRecTools/InDetRecToolInterfaces
                          Trigger/TrigSteer/TrigInterfaces
                          Trigger/TrigTools/TrigTimeAlgs
                          PRIVATE
                          DetectorDescription/IRegionSelector
                          Tracking/TrkEvent/TrkSegment
                          Trigger/TrigEvent/TrigParticle
                          Trigger/TrigEvent/TrigSteeringEvent )

# Component(s) in the package:
atlas_add_component( TRT_TrigTrackSegmentsFinder
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES GaudiKernel InDetRecToolInterfaces TrigInterfacesLib TrigTimeAlgsLib IRegionSelector TrkSegment TrigParticle TrigSteeringEvent )

# Install files from the package:
atlas_install_headers( TRT_TrigTrackSegmentsFinder )
atlas_install_python_modules( python/*.py )

