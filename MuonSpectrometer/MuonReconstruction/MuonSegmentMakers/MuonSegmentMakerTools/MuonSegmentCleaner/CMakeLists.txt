################################################################################
# Package: MuonSegmentCleaner
################################################################################

# Declare the package name:
atlas_subdir( MuonSegmentCleaner )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          GaudiKernel
                          MuonSpectrometer/MuonReconstruction/MuonRecTools/MuonRecToolInterfaces
                          PRIVATE
                          Control/CxxUtils
                          Event/EventPrimitives
                          MuonSpectrometer/MuonDetDescr/MuonReadoutGeometry
                          MuonSpectrometer/MuonIdHelpers
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonCompetingRIOsOnTrack
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonPrepRawData
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonRIO_OnTrack
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonSegment
                          Tracking/TrkDetDescr/TrkSurfaces
                          Tracking/TrkEvent/TrkCompetingRIOsOnTrack
                          Tracking/TrkEvent/TrkEventPrimitives
                          Tracking/TrkEvent/TrkPrepRawData
                          Tracking/TrkEvent/TrkRIO_OnTrack )

# External dependencies:
find_package( Eigen )

# Component(s) in the package:
atlas_add_library( MuonSegmentCleanerLib
                   src/*.cxx
                   PUBLIC_HEADERS MuonSegmentCleaner
                   PRIVATE_INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS}
                   LINK_LIBRARIES AthenaBaseComps GaudiKernel MuonRecToolInterfaces MuonIdHelpersLib
                   PRIVATE_LINK_LIBRARIES ${EIGEN_LIBRARIES} CxxUtils EventPrimitives MuonReadoutGeometry MuonCompetingRIOsOnTrack MuonPrepRawData MuonRIO_OnTrack MuonSegment TrkSurfaces TrkCompetingRIOsOnTrack TrkEventPrimitives TrkPrepRawData TrkRIO_OnTrack )

atlas_add_component( MuonSegmentCleaner
                     src/components/*.cxx
                     INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS}
                     LINK_LIBRARIES ${EIGEN_LIBRARIES} AthenaBaseComps GaudiKernel MuonRecToolInterfaces CxxUtils EventPrimitives MuonReadoutGeometry MuonIdHelpersLib MuonCompetingRIOsOnTrack MuonPrepRawData MuonRIO_OnTrack MuonSegment TrkSurfaces TrkCompetingRIOsOnTrack TrkEventPrimitives TrkPrepRawData TrkRIO_OnTrack MuonSegmentCleanerLib )

