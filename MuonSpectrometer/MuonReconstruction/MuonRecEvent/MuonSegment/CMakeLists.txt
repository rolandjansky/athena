################################################################################
# Package: MuonSegment
################################################################################

# Declare the package name:
atlas_subdir( MuonSegment )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/CLIDSvc
                          Control/DataModel
                          Control/SGTools
                          DetectorDescription/Identifier
                          Tracking/TrkDetDescr/TrkSurfaces
                          Tracking/TrkEvent/TrkEventPrimitives
                          Tracking/TrkEvent/TrkRIO_OnTrack
                          Tracking/TrkEvent/TrkSegment
                          Tracking/TrkEvent/TrkSpaceTimePoint
                          PRIVATE
                          Control/CxxUtils
                          DetectorDescription/GeoPrimitives
                          Event/EventPrimitives )

# External dependencies:
find_package( Eigen )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( MuonSegment
                   src/*.cxx
                   PUBLIC_HEADERS MuonSegment
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                   LINK_LIBRARIES DataModel SGTools Identifier TrkSurfaces TrkEventPrimitives TrkRIO_OnTrack TrkSegment TrkSpaceTimePoint
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} ${EIGEN_LIBRARIES} CxxUtils GeoPrimitives EventPrimitives )

