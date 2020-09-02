################################################################################
# Package: CscClusterization
################################################################################

# Declare the package name:
atlas_subdir( CscClusterization )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          GaudiKernel
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonPrepRawData
                          PRIVATE
                          Control/AthenaBaseComps
                          Event/EventPrimitives
                          MuonSpectrometer/MuonCalib/CscCalib/CscCalibTools
                          MuonSpectrometer/MuonDetDescr/MuonReadoutGeometry
                          MuonSpectrometer/MuonIdHelpers
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonRIO_OnTrack
                          Tracking/TrkDetDescr/TrkSurfaces
                          Tracking/TrkEvent/TrkEventPrimitives
                          Tracking/TrkEvent/TrkRIO_OnTrack )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( CscClusterizationLib
                   src/*.cxx
                   PUBLIC_HEADERS CscClusterization
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES GaudiKernel MuonPrepRawData MuonIdHelpersLib CscCalibToolsLib
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaBaseComps EventPrimitives MuonReadoutGeometry MuonRIO_OnTrack TrkSurfaces TrkEventPrimitives TrkRIO_OnTrack )

atlas_add_component( CscClusterization
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} GaudiKernel MuonPrepRawData AthenaBaseComps EventPrimitives MuonReadoutGeometry MuonIdHelpersLib MuonRIO_OnTrack TrkSurfaces TrkEventPrimitives TrkRIO_OnTrack CscClusterizationLib )

