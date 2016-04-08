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
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( TrigTrackJetFinderTool
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaBaseComps GaudiKernel Particle TrkTrack TrigInDetEvent )

# Install files from the package:
atlas_install_headers( TrigTrackJetFinderTool )

