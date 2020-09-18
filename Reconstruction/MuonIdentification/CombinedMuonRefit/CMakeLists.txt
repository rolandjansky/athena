################################################################################
# Package: CombinedMuonRefit
################################################################################

# Declare the package name:
atlas_subdir( CombinedMuonRefit )

# Component(s) in the package:
atlas_add_component( CombinedMuonRefit
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES GaudiKernel TrkTrack AthenaBaseComps MuonRecHelperToolsLib MuonRecToolInterfaces TrkParameters TrkExInterfaces )

# Install files from the package:
atlas_install_headers( CombinedMuonRefit )

