################################################################################
# Package: MCTruthClassifier
################################################################################

# Declare the package name:
atlas_subdir( MCTruthClassifier )

# Extra dependencies based on the environment:
set( extra_deps )
set( extra_libs )

if( XAOD_STANDALONE )
	set( extra_deps 
		Event/xAOD/xAODCaloEvent
		Event/xAOD/xAODEgamma
		Event/xAOD/xAODJet
		Event/xAOD/xAODMuon
		Event/xAOD/xAODTracking 
		Control/xAODRootAccess)
	set( extra_libs xAODCaloEvent xAODEgamma xAODJet xAODMuon xAODTracking  )

elseif( XAOD_ANALYSIS )
	set( extra_deps 
		Event/xAOD/xAODCaloEvent
		Event/xAOD/xAODEgamma
		Event/xAOD/xAODJet
		Event/xAOD/xAODMuon
		Event/xAOD/xAODTracking 
		GaudiKernel)
	set( extra_libs xAODCaloEvent xAODEgamma xAODJet xAODMuon xAODTracking)

elseif(GENERATIONBASE)
	set(extra_deps 
		GaudiKernel
		Generators/AtlasHepMC
		Generators/GeneratorObjects
		PRIVATE
		Control/AthenaKernel)
	set( extra_libs GeneratorObjects)

else()
	set( extra_deps 
		GaudiKernel
		Event/xAOD/xAODCaloEvent
		Event/xAOD/xAODEgamma
		Event/xAOD/xAODJet
		Event/xAOD/xAODMuon
		Event/xAOD/xAODTracking  
		Reconstruction/RecoTools/ParticlesInConeTools
		Reconstruction/RecoTools/RecoToolInterfaces
		Generators/AtlasHepMC
		Generators/GeneratorObjects
		PRIVATE
		Tracking/TrkEvent/TrkEventPrimitives
		Tracking/TrkEvent/TrkParametersIdentificationHelpers
		Control/AthenaKernel )
	set( extra_libs
		xAODCaloEvent xAODEgamma xAODJet xAODMuon xAODTracking 
		RecoToolInterfaces GeneratorObjects ParticlesInConeToolsLib
		PRIVATE_LINK_LIBRARIES TrkEventPrimitives TrkParametersIdentificationHelpers )

endif()

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   Control/AthToolSupport/AsgDataHandles
   Control/AthToolSupport/AsgTools
   Event/xAOD/xAODTruth
   Generators/TruthUtils
   Generators/AtlasHepMC
   ${extra_deps} )

# External dependencies:
find_package( ROOT COMPONENTS Core RIO )
find_package( HepMC )

# Libraries in the package:
if( HEPMC_FOUND )
	atlas_add_library( MCTruthClassifierLib
		MCTruthClassifier/*.h src/*.cxx Root/*.cxx
		PUBLIC_HEADERS MCTruthClassifier
		LINK_LIBRARIES AtlasHepMCLib AsgTools xAODTruth TruthUtils AsgDataHandlesLib ${extra_libs} )
else()
	atlas_add_library( MCTruthClassifierLib
		MCTruthClassifier/*.h src/*.cxx Root/*.cxx
		PUBLIC_HEADERS MCTruthClassifier
		LINK_LIBRARIES AsgTools xAODTruth TruthUtils AsgDataHandlesLib ${extra_libs} )
endif()

if( NOT XAOD_STANDALONE )
	atlas_add_component( MCTruthClassifier
		src/components/*.cxx
		LINK_LIBRARIES GaudiKernel MCTruthClassifierLib )
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
atlas_install_python_modules( python/*.py )
