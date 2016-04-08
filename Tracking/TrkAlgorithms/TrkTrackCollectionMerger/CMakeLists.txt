################################################################################
# Package: TrkTrackCollectionMerger
################################################################################

# Declare the package name:
atlas_subdir( TrkTrackCollectionMerger )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          GaudiKernel
                          Tracking/TrkEvent/TrkTrack
                          Tracking/TrkTools/TrkToolInterfaces
                          PRIVATE
                          Tracking/TrkEvent/TrkPrepRawData )

# Component(s) in the package:
atlas_add_component( TrkTrackCollectionMerger
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps GaudiKernel TrkTrack TrkToolInterfaces TrkPrepRawData )

# Install files from the package:
atlas_install_headers( TrkTrackCollectionMerger )

