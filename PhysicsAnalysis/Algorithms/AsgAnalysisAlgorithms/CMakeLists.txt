# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
#
# @author Nils Krumnack

atlas_subdir( AsgAnalysisAlgorithms )

atlas_depends_on_subdirs(
   PUBLIC
   Control/AthContainers
   Control/AthContainersInterfaces
   Control/AthToolSupport/AsgTools
   Event/xAOD/xAODBase
   Event/xAOD/xAODEventInfo
   PhysicsAnalysis/Algorithms/SelectionHelpers
   PhysicsAnalysis/Algorithms/SystematicsHandles
   PhysicsAnalysis/AnalysisCommon/PATCore
   PhysicsAnalysis/AnalysisCommon/PATInterfaces
   PhysicsAnalysis/AnalysisCommon/IsolationSelection
   PhysicsAnalysis/D3PDTools/AnaAlgorithm
   PhysicsAnalysis/Interfaces/AsgAnalysisInterfaces
   PRIVATE
   Event/xAOD/xAODCore
   Event/xAOD/xAODJet
   Event/xAOD/xAODMuon
   Event/xAOD/xAODTau
   Event/xAOD/xAODEgamma
   PhysicsAnalysis/D3PDTools/RootCoreUtils )

atlas_add_library( AsgAnalysisAlgorithmsLib
   AsgAnalysisAlgorithms/*.h AsgAnalysisAlgorithms/*.icc Root/*.cxx
   PUBLIC_HEADERS AsgAnalysisAlgorithms
   LINK_LIBRARIES AthContainers AthContainersInterfaces AsgTools xAODBase
     xAODEventInfo SelectionHelpersLib SystematicsHandlesLib PATCoreLib
     PATInterfaces AnaAlgorithmLib AsgAnalysisInterfaces AssociationUtilsLib
     IsolationSelectionLib
   PRIVATE_LINK_LIBRARIES xAODCore xAODJet xAODMuon xAODTau xAODEgamma
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
  atlas_add_test( EventAlgsTestJobData
     SCRIPT EventAlgorithmsTest_eljob.py --data-type data --unit-test
     PROPERTIES TIMEOUT 300 )
  atlas_add_test( EventAlgsTestJobFullSim
     SCRIPT EventAlgorithmsTest_eljob.py --data-type mc --unit-test
     PROPERTIES TIMEOUT 300 )
  atlas_add_test( EventAlgsTestJobFastSim
     SCRIPT EventAlgorithmsTest_eljob.py --data-type afii --unit-test
     PROPERTIES TIMEOUT 300 )
  atlas_add_test( OverlapRemovalTestJobData
     SCRIPT OverlapAlgorithmsTest_eljob.py --data-type data --unit-test
     PROPERTIES TIMEOUT 400 )
  atlas_add_test( OverlapRemovalTestJobFullSim
     SCRIPT OverlapAlgorithmsTest_eljob.py --data-type mc --unit-test
     PROPERTIES TIMEOUT 400 )
  atlas_add_test( OverlapRemovalTestJobFastSim
     SCRIPT OverlapAlgorithmsTest_eljob.py --data-type afii --unit-test
     PROPERTIES TIMEOUT 400 )
else()
   atlas_add_test( EventAlgsTestJob
      SCRIPT athena.py
             AsgAnalysisAlgorithms/EventAlgorithmsTest_jobOptions.py
      PROPERTIES TIMEOUT 300 )
   atlas_add_test( OverlapRemovalTestJob
      SCRIPT athena.py
             AsgAnalysisAlgorithms/OverlapAlgorithmsTest_jobOptions.py
      PROPERTIES TIMEOUT 400 )
endif()
