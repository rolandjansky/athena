################################################################################
# Package: SiSPSeededTrackFinder
################################################################################

# Declare the package name:
atlas_subdir( SiSPSeededTrackFinder )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/StoreGate
                          GaudiKernel
                          InnerDetector/InDetConditions/InDetBeamSpotService
                          InnerDetector/InDetRecTools/InDetRecToolInterfaces
                          Tracking/TrkDetDescr/TrkGeometry
                          Tracking/TrkDetDescr/TrkSurfaces
                          Tracking/TrkEvent/TrkSpacePoint
                          Tracking/TrkEvent/TrkTrack
                          Tracking/TrkExtrapolation/TrkExInterfaces
                          PRIVATE
                          Control/CxxUtils
                          Event/xAOD/xAODEventInfo
                          Tracking/TrkEvent/TrkPatternParameters
                          Tracking/TrkEvent/TrkRIO_OnTrack )

# Component(s) in the package:
atlas_add_component( SiSPSeededTrackFinder
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps StoreGateLib SGtests GaudiKernel InDetRecToolInterfaces TrkGeometry TrkSurfaces TrkSpacePoint TrkTrack TrkExInterfaces CxxUtils xAODEventInfo TrkPatternParameters TrkRIO_OnTrack )

# Install files from the package:
atlas_install_headers( SiSPSeededTrackFinder )

