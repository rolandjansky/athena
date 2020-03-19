################################################################################
# Package: TrigTrackJetFinderTool
################################################################################

# Declare the package name:
atlas_subdir( TrigTrackJetFinderTool )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          GaudiKernel
                          Reconstruction/Particle
                          Tracking/TrkEvent/TrkTrack
                          Trigger/TrigEvent/TrigInDetEvent )

# External dependencies:
find_package( ROOT COMPONENTS MathCore )

# Component(s) in the package:
atlas_add_library( TrigTrackJetFinderToolLib
                   TrigTrackJetFinderTool/*.h
                   INTERFACE
                   PUBLIC_HEADERS TrigTrackJetFinderTool
                   LINK_LIBRARIES AthenaBaseComps GaudiKernel Particle TrigInDetEvent TrkTrack )

atlas_add_component( TrigTrackJetFinderTool
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} TrigTrackJetFinderToolLib )
