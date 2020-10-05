################################################################################
# Package: MuonTruthOverlay
################################################################################

# Declare the package name:
atlas_subdir( MuonTruthOverlay )

# Component(s) in the package:
atlas_add_component( MuonTruthOverlay
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps GaudiKernel MuonSimData )

# Install files from the package:
atlas_install_headers( MuonTruthOverlay )
atlas_install_python_modules( python/*.py )
