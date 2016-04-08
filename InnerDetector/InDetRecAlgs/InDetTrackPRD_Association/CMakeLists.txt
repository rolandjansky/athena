################################################################################
# Package: InDetTrackPRD_Association
################################################################################

# Declare the package name:
atlas_subdir( InDetTrackPRD_Association )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          GaudiKernel
                          Tracking/TrkTools/TrkToolInterfaces
                          PRIVATE
                          Tracking/TrkEvent/TrkTrack )

# Component(s) in the package:
atlas_add_component( InDetTrackPRD_Association
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps GaudiKernel TrkToolInterfaces TrkTrack )

# Install files from the package:
atlas_install_headers( InDetTrackPRD_Association )

