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
                          Generators/GeneratorObjects )

# External dependencies:
find_package( HepMC )
find_package( MCTester )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread MathMore Minuit Minuit2 Matrix Physics HistPainter Rint Graf Graf3d Gpad Html Postscript Gui GX11TTF GX11 )

# Component(s) in the package:
atlas_add_component( MCTester_i
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${MCTESTER_INCLUDE_DIRS} ${HEPMC_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${MCTESTER_LIBRARIES} ${HEPMC_LIBRARIES} AthenaBaseComps GaudiKernel StoreGateLib SGtests EventInfo GeneratorObjects )

# Install files from the package:
atlas_install_headers( MCTester_i )
atlas_install_joboptions( share/*.py )
atlas_install_scripts( share/*.sh share/*.C )

