################################################################################
# Package: TrkEventUtils
################################################################################

# Declare the package name:
atlas_subdir( TrkEventUtils )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/CLIDSvc
                          Control/SGTools
                          DetectorDescription/GeoPrimitives
                          Event/EventPrimitives
                          Tracking/TrkDetDescr/TrkSurfaces
                          Tracking/TrkEvent/TrkEventPrimitives
                          Tracking/TrkEvent/TrkMeasurementBase
                          Tracking/TrkEvent/TrkParameters
                          Tracking/TrkEvent/TrkPrepRawData
                          Tracking/TrkEvent/TrkRIO_OnTrack
                          Tracking/TrkEvent/TrkTrack
                          PRIVATE
                          DetectorDescription/AtlasDetDescr
                          Tracking/TrkEvent/TrkCompetingRIOsOnTrack
                          Tracking/TrkEvent/TrkPseudoMeasurementOnTrack
                          Tracking/TrkEvent/TrkSegment
                          Tracking/TrkEvent/TrkSpacePoint
                          Tracking/TrkEvent/TrkVertexOnTrack )

# External dependencies:
find_package( Eigen )

# Component(s) in the package:
atlas_add_library( TrkEventUtils
                   src/*.cxx
                   PUBLIC_HEADERS TrkEventUtils
                   INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS}
                   LINK_LIBRARIES ${EIGEN_LIBRARIES} SGTools GeoPrimitives EventPrimitives TrkSurfaces TrkEventPrimitives TrkMeasurementBase TrkParameters TrkPrepRawData TrkRIO_OnTrack TrkTrack
                   PRIVATE_LINK_LIBRARIES AtlasDetDescr TrkCompetingRIOsOnTrack TrkPseudoMeasurementOnTrack TrkSegment TrkSpacePoint TrkVertexOnTrack )

