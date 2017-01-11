################################################################################
# Package: TrkTrackSlimmer
################################################################################

# Declare the package name:
atlas_subdir( TrkTrackSlimmer )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          GaudiKernel
                          Tracking/TrkEvent/TrkTrack
                          PRIVATE
                          Tracking/TrkTools/TrkToolInterfaces )

# Component(s) in the package:
atlas_add_component( TrkTrackSlimmer
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps GaudiKernel TrkTrack TrkToolInterfaces )

# Install files from the package:
atlas_install_headers( TrkTrackSlimmer )

