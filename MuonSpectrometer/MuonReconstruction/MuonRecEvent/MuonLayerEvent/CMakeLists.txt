################################################################################
# Package: MuonLayerEvent
################################################################################

# Declare the package name:
atlas_subdir( MuonLayerEvent )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthContainers
                          Event/xAOD/xAODCore
                          MuonSpectrometer/MuonIdHelpers
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonPrepRawData
                          Tracking/TrkEvent/TrkParameters
                          PRIVATE
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonRIO_OnTrack )

# Component(s) in the package:
atlas_add_library( MuonLayerEvent
                   src/*.cxx
                   PUBLIC_HEADERS MuonLayerEvent
                   LINK_LIBRARIES AthContainers xAODCore MuonPrepRawData TrkParameters MuonIdHelpersLib
                   PRIVATE_LINK_LIBRARIES MuonRIO_OnTrack )

