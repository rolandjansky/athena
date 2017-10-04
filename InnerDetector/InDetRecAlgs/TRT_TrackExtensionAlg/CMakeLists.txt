################################################################################
# Package: TRT_TrackExtensionAlg
################################################################################

# Declare the package name:
atlas_subdir( TRT_TrackExtensionAlg )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          GaudiKernel
                          InnerDetector/InDetRecTools/InDetRecToolInterfaces
                          PRIVATE
                          Control/StoreGate
                          Tracking/TrkEvent/TrkTrack )

# Component(s) in the package:
atlas_add_component( TRT_TrackExtensionAlg
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps GaudiKernel InDetRecToolInterfaces TrkTrack )

# Install files from the package:
atlas_install_headers( TRT_TrackExtensionAlg )

