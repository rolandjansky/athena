# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( MCTruthClassifier )

# Extra dependencies based on the environment:
set( extra_libs )

if( XAOD_STANDALONE )
	set( extra_libs xAODCaloEvent xAODEgamma xAODJet xAODMuon xAODRootAccess xAODTracking )

elseif( XAOD_ANALYSIS )
	set( extra_libs GaudiKernel xAODCaloEvent xAODEgamma xAODJet xAODMuon xAODTracking )

elseif(GENERATIONBASE)
	set( extra_libs GaudiKernel GeneratorObjects
       PRIVATE_LINK_LIBRARIES AthenaKernel )

else()
	set( extra_libs
	   GaudiKernel xAODCaloEvent xAODEgamma xAODJet xAODMuon xAODTracking
	   RecoToolInterfaces GeneratorObjects ParticlesInConeToolsLib
	   PRIVATE_LINK_LIBRARIES AthenaKernel TrkEventPrimitives TrkParametersIdentificationHelpers )

endif()

# External dependencies:
find_package( ROOT COMPONENTS Core RIO )
if ( HEPMC3_USE )
  find_package( hepmc3 )
  MESSAGE( STATUS "Trying HepMC3")
# Libraries in the package:
  if( HEPMC3_FOUND )
    MESSAGE( STATUS "HepMC3 FOUND!")
    list(INSERT extra_libs 0 AtlasHepMCLib )
  endif()
else()
  find_package( HepMC )
# Libraries in the package:
  if( HEPMC_FOUND )
    list(INSERT extra_libs 0 AtlasHepMCLib )
  endif()
endif()

atlas_add_library( MCTruthClassifierLib
   MCTruthClassifier/*.h src/*.cxx Root/*.cxx
   PUBLIC_HEADERS MCTruthClassifier
   LINK_LIBRARIES AsgTools xAODTruth TruthUtils AsgDataHandlesLib ${extra_libs} )

if( NOT XAOD_STANDALONE )
	atlas_add_component( MCTruthClassifier
		src/components/*.cxx
		LINK_LIBRARIES MCTruthClassifierLib )
endif()

atlas_add_dictionary( MCTruthClassifierDict
	MCTruthClassifier/MCTruthClassifierDict.h
	MCTruthClassifier/selection.xml
	LINK_LIBRARIES MCTruthClassifierLib )

# Executable(s) in the package:
if( XAOD_STANDALONE )
	atlas_add_executable( testClassifier
		util/testClassifier.cxx
		INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
		LINK_LIBRARIES ${ROOT_LIBRARIES} xAODRootAccess xAODEgamma
		MCTruthClassifierLib )
endif()

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
