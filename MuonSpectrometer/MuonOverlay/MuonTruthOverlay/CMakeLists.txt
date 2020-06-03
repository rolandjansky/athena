################################################################################
# Package: MuonTruthOverlay
################################################################################

# Declare the package name:
atlas_subdir( MuonTruthOverlay )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          GaudiKernel
                          Control/AthenaBaseComps
                          PRIVATE
                          MuonSpectrometer/MuonSimData )

# Component(s) in the package:
atlas_add_component( MuonTruthOverlay
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps GaudiKernel MuonSimData )

# Install files from the package:
atlas_install_headers( MuonTruthOverlay )
atlas_install_python_modules( python/*.py )
