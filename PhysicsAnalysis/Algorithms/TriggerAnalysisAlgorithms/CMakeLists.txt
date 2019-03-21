# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#
# @author Tadej Novak

atlas_subdir( TriggerAnalysisAlgorithms )

atlas_depends_on_subdirs(
   PUBLIC
   Event/xAOD/xAODEventInfo
   PhysicsAnalysis/Algorithms/SelectionHelpers
   PhysicsAnalysis/Algorithms/SystematicsHandles
   PhysicsAnalysis/D3PDTools/AnaAlgorithm
   Trigger/TrigEvent/TrigDecisionInterface )

atlas_add_library( TriggerAnalysisAlgorithmsLib
   TriggerAnalysisAlgorithms/*.h TriggerAnalysisAlgorithms/*.icc Root/*.cxx
   PUBLIC_HEADERS TriggerAnalysisAlgorithms
   LINK_LIBRARIES xAODEventInfo SelectionHelpersLib SystematicsHandlesLib AnaAlgorithmLib )

atlas_add_dictionary( TriggerAnalysisAlgorithmsDict
   TriggerAnalysisAlgorithms/TriggerAnalysisAlgorithmsDict.h
   TriggerAnalysisAlgorithms/selection.xml
   LINK_LIBRARIES TriggerAnalysisAlgorithmsLib )

if( NOT XAOD_STANDALONE )
   atlas_add_component( TriggerAnalysisAlgorithms
      src/*.h src/*.cxx src/components/*.cxx
      LINK_LIBRARIES GaudiKernel TriggerAnalysisAlgorithmsLib )
endif()

atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*_jobOptions.py )
atlas_install_scripts( share/*_eljob.py )

if( XAOD_STANDALONE )
  atlas_add_test( TriggerAlgorithmsTestJobData
     SCRIPT TriggerAlgorithmsTest_eljob.py --data-type data --unit-test
     PROPERTIES TIMEOUT 300 )
  atlas_add_test( TriggerAlgorithmsTestJobFullSim
     SCRIPT TriggerAlgorithmsTest_eljob.py --data-type mc --unit-test
     PROPERTIES TIMEOUT 300 )
  atlas_add_test( TriggerAlgorithmsTestJobFastSim
     SCRIPT TriggerAlgorithmsTest_eljob.py --data-type afii --unit-test
     PROPERTIES TIMEOUT 300 )
else()
   atlas_add_test( TriggerAlgorithmsTestJob
      SCRIPT athena.py
             TriggerAnalysisAlgorithms/TriggerAlgorithmsTest_jobOptions.py
      PROPERTIES TIMEOUT 300 )
endif()
