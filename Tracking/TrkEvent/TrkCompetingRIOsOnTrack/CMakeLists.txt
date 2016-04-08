################################################################################
# Package: TrkCompetingRIOsOnTrack
################################################################################

# Declare the package name:
atlas_subdir( TrkCompetingRIOsOnTrack )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          DetectorDescription/GeoPrimitives
                          Event/EventPrimitives
                          Tracking/TrkEvent/TrkEventPrimitives
                          Tracking/TrkEvent/TrkMeasurementBase
                          PRIVATE
                          Tracking/TrkEvent/TrkRIO_OnTrack )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( Eigen )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( TrkCompetingRIOsOnTrack
                   src/*.cxx
                   PUBLIC_HEADERS TrkCompetingRIOsOnTrack
                   INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${EIGEN_LIBRARIES} GeoPrimitives EventPrimitives TrkEventPrimitives TrkMeasurementBase
                   PRIVATE_LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} TrkRIO_OnTrack )

atlas_add_dictionary( TrkCompetingRIOsOnTrackDict
                      TrkCompetingRIOsOnTrack/TrkCompetingRIOsOnTrackDict.h
                      TrkCompetingRIOsOnTrack/selection.xml
                      INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                      LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} ${EIGEN_LIBRARIES} GeoPrimitives EventPrimitives TrkEventPrimitives TrkMeasurementBase TrkRIO_OnTrack TrkCompetingRIOsOnTrack )

