################################################################################
# Package: HepMCAnalysis_i
################################################################################

# Declare the package name:
atlas_subdir( HepMCAnalysis_i )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          GaudiKernel
                          PRIVATE
                          Control/StoreGate
                          Event/EventInfo
                          Generators/GeneratorObjects
                          Generators/TruthUtils
                          TestPolicy )

# External dependencies:
find_package( CLHEP )
find_package( HEPUtils )
find_package( HepMC )
find_package( HepMCAnalysis )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( HepMCAnalysis_i
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${HEPMCANALYSIS_INCLUDE_DIRS} ${HEPUTILS_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${HEPMC_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${HEPMCANALYSIS_LIBRARIES} ${HEPUTILS_LIBRARIES} ${CLHEP_LIBRARIES} ${HEPMC_LIBRARIES} AthenaBaseComps GaudiKernel StoreGateLib SGtests EventInfo GeneratorObjects TruthUtils )

# Install files from the package:
atlas_install_headers( HepMCAnalysis_i )
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )
atlas_install_runtime( test/*.xml share/*.C share/*.py share/common/*.py share/RTTJO/HepMCAnalysisJO_*.py )

