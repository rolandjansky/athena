################################################################################
# Package: MCTester_i
################################################################################

# Declare the package name:
atlas_subdir( MCTester_i )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          GaudiKernel
                          PRIVATE
                          Control/StoreGate
                          Event/EventInfo
                          Generators/AtlasHepMC
                          Generators/GeneratorObjects )

# External dependencies:
find_package( MCTester )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread MathMore Minuit Minuit2 Matrix Physics HistPainter Rint Graf Graf3d Gpad Html Postscript Gui GX11TTF GX11 )
if (HEPMC3_USE)
set( MCTester_sources src/MCTesterAlg.cxx 
                      ${HEPMC3_INCLUDE_DIR}/../share/HepMC3/interfaces/mc-tester/src/HepMC3Particle.cxx 
                      ${HEPMC3_INCLUDE_DIR}/../share/HepMC3/interfaces/mc-tester/src/HepMC3Event.cxx) 
set( HEPMC_MCTester_INTERFACE_DIR ${HEPMC3_INCLUDE_DIR}/../share/HepMC3/interfaces/mc-tester/include)
else()
set( MCTester_sources src/MCTesterAlg.cxx)
set( HEPMC_MCTester_INTERFACE_DIR ${MCTESTER_INCLUDE_DIRS})
endif()

# Component(s) in the package:
atlas_add_component( MCTester_i
                     ${MCTester_sources}
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${MCTESTER_INCLUDE_DIRS} ${HEPMC_MCTester_INTERFACE_DIR}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${MCTESTER_LIBRARIES} AtlasHepMCLib AthenaBaseComps GaudiKernel StoreGateLib SGtests EventInfo GeneratorObjects )

# Install files from the package:
atlas_install_headers( MCTester_i )
atlas_install_joboptions( share/*.py )
atlas_install_scripts( share/*.sh share/*.C )

