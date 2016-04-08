################################################################################
# Package: MuidCaloIsolationTools
################################################################################

# Declare the package name:
atlas_subdir( MuidCaloIsolationTools )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          GaudiKernel
                          Reconstruction/MuonIdentification/MuidInterfaces
                          Tracking/TrkEvent/TrkTrack
                          PRIVATE
                          Tracking/TrkDetDescr/TrkSurfaces
                          Tracking/TrkEvent/TrkParameters
                          Tracking/TrkExtrapolation/TrkExInterfaces
                          Tracking/TrkExtrapolation/TrkExUtils )

# Component(s) in the package:
atlas_add_component( MuidCaloIsolationTools
                     src/MuidTrackIsolation.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps GaudiKernel MuidInterfaces TrkTrack TrkSurfaces TrkParameters TrkExInterfaces TrkExUtils )

# Install files from the package:
atlas_install_headers( MuidCaloIsolationTools )

