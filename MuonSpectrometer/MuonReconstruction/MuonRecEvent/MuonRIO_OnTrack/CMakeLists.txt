################################################################################
# Package: MuonRIO_OnTrack
################################################################################

# Declare the package name:
atlas_subdir( MuonRIO_OnTrack )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthLinks
                          GaudiKernel
                          MuonSpectrometer/MuonDetDescr/MuonReadoutGeometry
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonPrepRawData
                          Tracking/TrkDetDescr/TrkSurfaces
                          Tracking/TrkEvent/TrkEventPrimitives
                          Tracking/TrkEvent/TrkRIO_OnTrack
                          PRIVATE
                          DetectorDescription/GeoPrimitives )

# External dependencies:
find_package( Eigen )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( MuonRIO_OnTrack
                   src/*.cxx
                   PUBLIC_HEADERS MuonRIO_OnTrack
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                   LINK_LIBRARIES AthLinks GaudiKernel MuonReadoutGeometry MuonPrepRawData TrkSurfaces TrkEventPrimitives TrkRIO_OnTrack
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} ${EIGEN_LIBRARIES} GeoPrimitives )

atlas_add_dictionary( MuonRIO_OnTrackDict
                      MuonRIO_OnTrack/MuonRIO_OnTrackDict.h
                      MuonRIO_OnTrack/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} ${EIGEN_LIBRARIES} AthLinks GaudiKernel MuonReadoutGeometry MuonPrepRawData TrkSurfaces TrkEventPrimitives TrkRIO_OnTrack GeoPrimitives MuonRIO_OnTrack )

