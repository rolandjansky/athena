################################################################################
# Package: MuonLinearSegmentMakerUtilities
################################################################################

# Declare the package name:
atlas_subdir( MuonLinearSegmentMakerUtilities )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/DataModel
                          DetectorDescription/GeoPrimitives
                          GaudiKernel
                          MuonSpectrometer/MuonIdHelpers
                          PRIVATE
                          Event/EventPrimitives
                          MuonSpectrometer/MuonDetDescr/MuonReadoutGeometry
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonSegment
                          Tracking/TrkDetDescr/TrkSurfaces
                          Tracking/TrkEvent/TrkEventPrimitives
                          Tracking/TrkEvent/TrkRIO_OnTrack )

# External dependencies:
find_package( Eigen )
find_package( GSL )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( MuonLinearSegmentMakerUtilities
                   src/*.cxx
                   PUBLIC_HEADERS MuonLinearSegmentMakerUtilities
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${GSL_INCLUDE_DIRS}
                   LINK_LIBRARIES ${ROOT_LIBRARIES} ${EIGEN_LIBRARIES} DataModel GeoPrimitives GaudiKernel MuonIdHelpersLib
                   PRIVATE_LINK_LIBRARIES ${GSL_LIBRARIES} EventPrimitives MuonReadoutGeometry MuonSegment TrkSurfaces TrkEventPrimitives TrkRIO_OnTrack )

