################################################################################
# Package: SiSPSeededTrackFinder
################################################################################

# Declare the package name:
atlas_subdir( SiSPSeededTrackFinder )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          GaudiKernel
                          InnerDetector/InDetConditions/InDetBeamSpotService
                          InnerDetector/InDetRecTools/InDetRecToolInterfaces
                          Tracking/TrkDetDescr/TrkGeometry
                          Tracking/TrkDetDescr/TrkSurfaces
                          Tracking/TrkExtrapolation/TrkExInterfaces
                          PRIVATE
                          Event/xAOD/xAODEventInfo
                          Tracking/TrkEvent/TrkPatternParameters
                          Tracking/TrkEvent/TrkRIO_OnTrack
                          Tracking/TrkEvent/TrkSpacePoint
                          Tracking/TrkEvent/TrkTrack )

# Component(s) in the package:
atlas_add_component( SiSPSeededTrackFinder
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps GaudiKernel InDetRecToolInterfaces TrkGeometry TrkSurfaces TrkExInterfaces xAODEventInfo TrkPatternParameters TrkRIO_OnTrack TrkSpacePoint TrkTrack )

# Install files from the package:
atlas_install_headers( SiSPSeededTrackFinder )

