################################################################################
# Package: BoostedTopD3PDMaker
################################################################################

# Declare the package name:
atlas_subdir( BoostedTopD3PDMaker )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Event/EventKernel
                          GaudiKernel
                          PhysicsAnalysis/D3PDMaker/D3PDMakerUtils
                          PRIVATE
                          Control/AthenaKernel
                          Reconstruction/MuonIdentification/muonEvent
                          Reconstruction/Particle
                          Reconstruction/RecoTools/ITrackToVertex
                          Reconstruction/RecoTools/IsolationTool
                          Reconstruction/egamma/egammaEvent
                          Reconstruction/egamma/egammaInterfaces )

# Component(s) in the package:
atlas_add_component( BoostedTopD3PDMaker
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES EventKernel GaudiKernel D3PDMakerUtils AthenaKernel muonEvent Particle ITrackToVertex IsolationToolLib egammaEvent )

# Install files from the package:
atlas_install_headers( BoostedTopD3PDMaker )
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

