################################################################################
# Package: TRT_TrigStandaloneTrackFinder
################################################################################

# Declare the package name:
atlas_subdir( TRT_TrigStandaloneTrackFinder )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          GaudiKernel
                          Tracking/TrkEvent/TrkSegment
                          Trigger/TrigSteer/TrigInterfaces
                          PRIVATE
                          DetectorDescription/GeoPrimitives
                          Event/EventPrimitives
                          InnerDetector/InDetDetDescr/InDetIdentifier
                          InnerDetector/InDetRecEvent/InDetRIO_OnTrack
                          InnerDetector/InDetRecTools/InDetRecToolInterfaces
                          Tracking/TrkEvent/TrkEventPrimitives
                          Tracking/TrkEvent/TrkParameters
                          Tracking/TrkEvent/TrkPseudoMeasurementOnTrack
                          Tracking/TrkEvent/TrkTrack
                          Trigger/TrigEvent/TrigParticle )

# External dependencies:
find_package( Eigen )

# Component(s) in the package:
atlas_add_component( TRT_TrigStandaloneTrackFinder
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS}
                     LINK_LIBRARIES ${EIGEN_LIBRARIES} GaudiKernel TrkSegment TrigInterfacesLib GeoPrimitives EventPrimitives InDetIdentifier InDetRIO_OnTrack InDetRecToolInterfaces TrkEventPrimitives TrkParameters TrkPseudoMeasurementOnTrack TrkTrack TrigParticle )

# Install files from the package:
atlas_install_headers( TRT_TrigStandaloneTrackFinder )
atlas_install_python_modules( python/*.py )

