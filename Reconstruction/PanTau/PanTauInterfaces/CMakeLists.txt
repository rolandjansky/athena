################################################################################
# Package: PanTauInterfaces
################################################################################

# Declare the package name:
atlas_subdir( PanTauInterfaces )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Event/NavFourMom
                          Event/xAOD/xAODPFlow
                          Event/xAOD/xAODTau
                          GaudiKernel
                          Reconstruction/PanTau/PanTauEvent )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Install files from the package:
atlas_install_headers( PanTauInterfaces )

