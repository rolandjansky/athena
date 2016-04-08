################################################################################
# Package: CombinedMuonRefit
################################################################################

# Declare the package name:
atlas_subdir( CombinedMuonRefit )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          GaudiKernel
                          Tracking/TrkEvent/TrkTrack
                          PRIVATE
                          Control/AthenaBaseComps
                          MuonSpectrometer/MuonReconstruction/MuonRecTools/MuonRecHelperTools
                          MuonSpectrometer/MuonReconstruction/MuonRecTools/MuonRecToolInterfaces
                          Tracking/TrkEvent/TrkParameters
                          Tracking/TrkExtrapolation/TrkExInterfaces )

# Component(s) in the package:
atlas_add_component( CombinedMuonRefit
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES GaudiKernel TrkTrack AthenaBaseComps MuonRecHelperToolsLib MuonRecToolInterfaces TrkParameters TrkExInterfaces )

# Install files from the package:
atlas_install_headers( CombinedMuonRefit )

