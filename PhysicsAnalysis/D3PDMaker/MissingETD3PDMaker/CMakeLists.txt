################################################################################
# Package: MissingETD3PDMaker
################################################################################

# Declare the package name:
atlas_subdir( MissingETD3PDMaker )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Calorimeter/CaloEvent
                          Control/StoreGate
                          Event/EventKernel
                          GaudiKernel
                          PhysicsAnalysis/D3PDMaker/D3PDMakerInterfaces
                          PhysicsAnalysis/D3PDMaker/D3PDMakerUtils
                          Reconstruction/Jet/JetEvent
                          Reconstruction/MissingETEvent
                          Reconstruction/MissingETGoodness
                          Reconstruction/MissingETPerformance
                          Reconstruction/Particle
                          PRIVATE
                          Control/AthenaKernel
                          Control/SGTools
                          Event/FourMomUtils
                          Event/xAOD/xAODMissingET
                          Reconstruction/MuonIdentification/muonEvent
                          Reconstruction/egamma/egammaEvent
                          Reconstruction/tauEvent )

# External dependencies:
find_package( CLHEP )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( MissingETD3PDMaker
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} CaloEvent StoreGateLib SGtests EventKernel GaudiKernel D3PDMakerUtils JetEvent MissingETEvent MissingETGoodnessLib MissingETPerformanceLib Particle AthenaKernel SGTools FourMomUtils xAODMissingET muonEvent egammaEvent tauEvent )

# Install files from the package:
atlas_install_headers( MissingETD3PDMaker )
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

