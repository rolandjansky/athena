################################################################################
# Package: HepMCAnalysis_i
################################################################################

# Declare the package name:
atlas_subdir( HepMCAnalysis_i )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   Control/AthenaBaseComps
   GaudiKernel
   PRIVATE
   Control/StoreGate
   Event/EventInfo
   Generators/GeneratorObjects
   Generators/TruthUtils )

# External dependencies:
find_package( CLHEP )
find_package( HEPUtils )
find_package( HepMC )
find_package( HepMCAnalysis )
find_package( ROOT COMPONENTS Core MathCore Hist RIO )
find_package( FastJet )

# Component(s) in the package:
atlas_add_component( HepMCAnalysis_i
   HepMCAnalysis_i/*.h src/*.cxx src/components/*.cxx
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${HEPMCANALYSIS_INCLUDE_DIRS}
   ${HEPUTILS_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${HEPMC_INCLUDE_DIRS}
   ${FASTJET_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} ${HEPMCANALYSIS_LIBRARIES}
   ${HEPUTILS_LIBRARIES} ${CLHEP_LIBRARIES} ${HEPMC_LIBRARIES}
   ${FASTJET_LIBRARIES} AthenaBaseComps GaudiKernel StoreGateLib EventInfo
   GeneratorObjects TruthUtils )

# Install files from the package:
atlas_install_headers( HepMCAnalysis_i )
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )
atlas_install_runtime( test/*.xml share/*.C share/*.py share/common/*.py
   share/RTTJO/HepMCAnalysisJO_*.py )
