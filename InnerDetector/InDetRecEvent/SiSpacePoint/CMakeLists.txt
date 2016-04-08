################################################################################
# Package: SiSpacePoint
################################################################################

# Declare the package name:
atlas_subdir( SiSpacePoint )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          DetectorDescription/Identifier
                          Tracking/TrkDetDescr/TrkDetElementBase
                          Tracking/TrkEvent/TrkSpacePoint
                          PRIVATE
                          DetectorDescription/GeoPrimitives
                          Event/EventPrimitives
                          Tracking/TrkDetDescr/TrkSurfaces
                          Tracking/TrkEvent/TrkEventPrimitives
                          Tracking/TrkEvent/TrkPrepRawData )

# External dependencies:
find_package( Eigen )

# Component(s) in the package:
atlas_add_library( SiSpacePoint
                   src/*.cxx
                   PUBLIC_HEADERS SiSpacePoint
                   PRIVATE_INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS}
                   LINK_LIBRARIES Identifier TrkDetElementBase TrkSpacePoint
                   PRIVATE_LINK_LIBRARIES ${EIGEN_LIBRARIES} GeoPrimitives EventPrimitives TrkSurfaces TrkEventPrimitives TrkPrepRawData )

