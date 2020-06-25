# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#
# @author Nils Krumnack

atlas_subdir( AsgAnalysisAlgorithms )

atlas_depends_on_subdirs(
   PUBLIC
   Control/AthContainers
   Control/AthContainersInterfaces
   Control/AthToolSupport/AsgTools
   Event/xAOD/xAODBase
   Event/xAOD/xAODCutFlow
   Event/xAOD/xAODEventInfo
   Event/xAOD/xAODMetaData
   PhysicsAnalysis/Algorithms/SelectionHelpers
   PhysicsAnalysis/Algorithms/SystematicsHandles
   PhysicsAnalysis/AnalysisCommon/PATCore
   PhysicsAnalysis/AnalysisCommon/PATInterfaces
   PhysicsAnalysis/AnalysisCommon/IsolationSelection
   PhysicsAnalysis/D3PDTools/AnaAlgorithm
   PhysicsAnalysis/Interfaces/AsgAnalysisInterfaces
   PhysicsAnalysis/Interfaces/PMGAnalysisInterfaces
   PRIVATE
   Event/xAOD/xAODCore
   Event/xAOD/xAODJet
   Event/xAOD/xAODMuon
   Event/xAOD/xAODTau
   Event/xAOD/xAODEgamma
   Event/xAOD/xAODTracking
   Event/xAOD/xAODTruth
   PhysicsAnalysis/D3PDTools/RootCoreUtils )

atlas_add_library( AsgAnalysisAlgorithmsLib
   AsgAnalysisAlgorithms/*.h AsgAnalysisAlgorithms/*.icc Root/*.cxx
   PUBLIC_HEADERS AsgAnalysisAlgorithms
   LINK_LIBRARIES AthContainers AthContainersInterfaces AsgTools xAODBase
     xAODEventInfo SelectionHelpersLib SystematicsHandlesLib PATCoreLib
     PATInterfaces AnaAlgorithmLib AsgAnalysisInterfaces AssociationUtilsLib
     IsolationSelectionLib
   PRIVATE_LINK_LIBRARIES xAODCore xAODCutFlow xAODMetaData xAODJet xAODMuon xAODTau xAODEgamma xAODTracking xAODTruth
     RootCoreUtils )

atlas_add_dictionary( AsgAnalysisAlgorithmsDict
   AsgAnalysisAlgorithms/AsgAnalysisAlgorithmsDict.h
   AsgAnalysisAlgorithms/selection.xml
   LINK_LIBRARIES AsgAnalysisAlgorithmsLib )

if( NOT XAOD_STANDALONE )
   atlas_add_component( AsgAnalysisAlgorithms
      src/*.h src/*.cxx src/components/*.cxx
      LINK_LIBRARIES GaudiKernel AsgAnalysisAlgorithmsLib )
endif()

atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*_jobOptions.py )
atlas_install_scripts( share/*_eljob.py )

if( XAOD_STANDALONE )
  # AnalysisBase migration: disabled until GRLSelectorAlg is added
  #atlas_add_test( EventAlgsTestJobData
  #   SCRIPT EventAlgorithmsTest_eljob.py --data-type data --unit-test
  #   PROPERTIES TIMEOUT 600 )
  atlas_add_test( EventAlgsTestJobFullSim
     SCRIPT EventAlgorithmsTest_eljob.py --data-type mc --unit-test
     PROPERTIES TIMEOUT 600 )
  atlas_add_test( EventAlgsTestJobFastSim
     SCRIPT EventAlgorithmsTest_eljob.py --data-type afii --unit-test
     PROPERTIES TIMEOUT 600 )

  # AnalysisBase migration: disabled until EgammaAnalysisAlgorithms is added
  #atlas_add_test( OverlapRemovalTestJobData
  #   SCRIPT OverlapAlgorithmsTest_eljob.py --data-type data --unit-test
  #   PROPERTIES TIMEOUT 600 )
  #atlas_add_test( OverlapRemovalTestJobFullSim
  #   SCRIPT OverlapAlgorithmsTest_eljob.py --data-type mc --unit-test
  #   PROPERTIES TIMEOUT 600 )
  #atlas_add_test( OverlapRemovalTestJobFastSim
  #   SCRIPT OverlapAlgorithmsTest_eljob.py --data-type afii --unit-test
  #   PROPERTIES TIMEOUT 600 )

  # AnalysisBase migration: disabled until PileupReweightingTool is updated
  #atlas_add_test( GeneratorAlgsTestJobFullSim
  #   SCRIPT GeneratorAlgorithmsTest_eljob.py --data-type mc --unit-test
  #   PROPERTIES TIMEOUT 600 )

else()

  atlas_add_test( EventAlgsTestJobData
      SCRIPT athena.py
             AsgAnalysisAlgorithms/EventAlgorithmsTest_jobOptions.py - --data-type data
      PROPERTIES TIMEOUT 600 )
   atlas_add_test( EventAlgsTestJobFullSim
      SCRIPT athena.py
             AsgAnalysisAlgorithms/EventAlgorithmsTest_jobOptions.py - --data-type mc
      PROPERTIES TIMEOUT 600 )
   atlas_add_test( EventAlgsTestJobFastSim
      SCRIPT athena.py
             AsgAnalysisAlgorithms/EventAlgorithmsTest_jobOptions.py - --data-type afii
      PROPERTIES TIMEOUT 600 )
   atlas_add_test( GeneratorAlgsTestJob
      SCRIPT athena.py
               AsgAnalysisAlgorithms/GeneratorAlgorithmsTest_jobOptions.py - --data-type mc
      PROPERTIES TIMEOUT 600 )

   if( NOT "${CMAKE_PROJECT_NAME}" STREQUAL "AthDerivation" )
      atlas_add_test( OverlapRemovalTestJobData
         SCRIPT athena.py
                AsgAnalysisAlgorithms/OverlapAlgorithmsTest_jobOptions.py - --data-type data
         PROPERTIES TIMEOUT 600 )
      atlas_add_test( OverlapRemovalTestJobFullSim
         SCRIPT athena.py
                AsgAnalysisAlgorithms/OverlapAlgorithmsTest_jobOptions.py - --data-type mc
         PROPERTIES TIMEOUT 600 )
      atlas_add_test( OverlapRemovalTestJobFastSim
         SCRIPT athena.py
                AsgAnalysisAlgorithms/OverlapAlgorithmsTest_jobOptions.py - --data-type afii
         PROPERTIES TIMEOUT 600 )
   endif()
endif()
