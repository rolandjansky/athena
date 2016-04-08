################################################################################
# Package: TrkRIO_OnTrack
################################################################################

# Declare the package name:
atlas_subdir( TrkRIO_OnTrack )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/CLIDSvc
                          Control/SGTools
                          DetectorDescription/GeoPrimitives
                          DetectorDescription/Identifier
                          Event/EventPrimitives
                          Tracking/TrkEvent/TrkEventPrimitives
                          Tracking/TrkEvent/TrkMeasurementBase
                          PRIVATE
                          GaudiKernel
                          Tracking/TrkEvent/TrkPrepRawData )

# External dependencies:
find_package( Eigen )

# Component(s) in the package:
atlas_add_library( TrkRIO_OnTrack
                   src/*.cxx
                   PUBLIC_HEADERS TrkRIO_OnTrack
                   INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS}
                   LINK_LIBRARIES ${EIGEN_LIBRARIES} SGTools GeoPrimitives Identifier EventPrimitives TrkEventPrimitives TrkMeasurementBase
                   PRIVATE_LINK_LIBRARIES GaudiKernel TrkPrepRawData )

