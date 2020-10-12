# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( HepMCAnalysis_i )

# External dependencies:
find_package( CLHEP )
find_package( HEPUtils )
find_package( HepMCAnalysis )
find_package( ROOT COMPONENTS Core MathCore Hist RIO )
find_package( FastJet )
find_package( HepMC )
if (HEPMC3_USE)
set( HEPMC_HepMCAnalysis_INTERFACE_DIR  ${HEPMC3_INCLUDE_DIR}/../share/HepMC3/interfaces/HepMCCompatibility/include )
else()
set( HEPMC_HepMCAnalysis_INTERFACE_DIR  )
endif()

# Component(s) in the package:
atlas_add_component( HepMCAnalysis_i
   HepMCAnalysis_i/*.h src/*.cxx src/components/*.cxx
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${HEPMCANALYSIS_INCLUDE_DIRS}
   ${HEPUTILS_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} 
   ${FASTJET_INCLUDE_DIRS}
   ${HEPMC_INCLUDE_DIRS}
   ${HEPMC_HepMCAnalysis_INTERFACE_DIR}
   LINK_LIBRARIES ${ROOT_LIBRARIES} ${HEPMCANALYSIS_LIBRARIES}
   ${HEPUTILS_LIBRARIES} ${CLHEP_LIBRARIES} AtlasHepMCLib
   ${FASTJET_LIBRARIES} AthenaBaseComps GaudiKernel StoreGateLib EventInfo
   GeneratorObjects TruthUtils )

# Install files from the package:
atlas_install_headers( HepMCAnalysis_i )
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )
atlas_install_runtime( test/*.xml share/*.C share/*.py share/common/*.py
   share/RTTJO/HepMCAnalysisJO_*.py )
