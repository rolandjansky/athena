# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( MCTruthClassifier )

# Extra dependencies based on the environment:
set( extra_libs )
if( XAOD_ANALYSIS )
   set( extra_libs xAODCaloEvent xAODEgamma xAODJet xAODMuon xAODRootAccess
      xAODTracking )
   if( NOT XAOD_STANDALONE )
      list( APPEND extra_libs GaudiKernel AtlasHepMCLib )
   endif()
elseif( GENERATIONBASE )
   set( extra_libs GaudiKernel GeneratorObjects AtlasHepMCLib
      PRIVATE_LINK_LIBRARIES AthenaKernel )
else()
   set( extra_libs GaudiKernel xAODCaloEvent xAODEgamma xAODJet xAODMuon
      xAODTracking RecoToolInterfaces GeneratorObjects ParticlesInConeToolsLib
      AtlasHepMCLib CaloDetDescrLib
      PRIVATE_LINK_LIBRARIES AthenaKernel TrkEventPrimitives
      TrkParametersIdentificationHelpers )
endif()

# Component(s) in the package.
atlas_add_library( MCTruthClassifierLib
   MCTruthClassifier/*.h src/*.cxx Root/*.cxx
   PUBLIC_HEADERS MCTruthClassifier
   LINK_LIBRARIES AsgTools xAODTruth TruthUtils AsgDataHandlesLib
	${extra_libs} )

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

#Tests
if( ( NOT XAOD_ANALYSIS ) AND ( NOT GENERATIONBASE ) )
   atlas_add_test( MCTruthClassifierConfigTest
      SCRIPT python -m MCTruthClassifier.MCTruthClassifierConfig
      POST_EXEC_SCRIPT nopost.sh )
endif()
