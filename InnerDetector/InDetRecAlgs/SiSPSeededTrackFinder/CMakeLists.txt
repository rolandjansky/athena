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
                          InnerDetector/InDetConditions/BeamSpotConditionsData
                          InnerDetector/InDetRecTools/InDetRecToolInterfaces
                          Tracking/TrkDetDescr/TrkGeometry
                          Tracking/TrkDetDescr/TrkSurfaces
                          Tracking/TrkEvent/TrkSpacePoint
                          Tracking/TrkEvent/TrkTrack
                          Tracking/TrkEvent/TrkEventUtils
                          Tracking/TrkExtrapolation/TrkExInterfaces
                          PRIVATE
                          Control/CxxUtils
                          Event/xAOD/xAODEventInfo
                          InnerDetector/InDetRecEvent/SiSPSeededTrackFinderData
                          Tracking/TrkEvent/TrkPatternParameters
                          Tracking/TrkEvent/TrkRIO_OnTrack )

# Component(s) in the package:
atlas_add_component( SiSPSeededTrackFinder
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps StoreGateLib SGtests GaudiKernel InDetRecToolInterfaces TrkGeometry TrkSurfaces TrkSpacePoint TrkTrack TrkExInterfaces CxxUtils xAODEventInfo SiSPSeededTrackFinderData TrkPatternParameters TrkRIO_OnTrack TrkEventUtils)

# Run tests:
atlas_add_test( SiSPSeededTracksStandalone
                SCRIPT athena.py --threads=5 SiSPSeededTrackFinder/SiSPSeededTracksStandalone.py
                PROPERTIES TIMEOUT 600
                ENVIRONMENT THREADS=5 )

atlas_add_test( SiSPSeededTracksStandaloneFromESD
                SCRIPT athena.py --threads=1 SiSPSeededTrackFinder/SiSPSeededTracksStandaloneFromESD.py
                PROPERTIES TIMEOUT 600
                ENVIRONMENT THREADS=5 )

# Install files from the package:
atlas_install_headers( SiSPSeededTrackFinder )
atlas_install_joboptions( share/*.py )
