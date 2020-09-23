# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
#
# @author Nils Krumnack


# The name of the package:
atlas_subdir( TauAnalysisAlgorithms )

atlas_add_library( TauAnalysisAlgorithmsLib
   TauAnalysisAlgorithms/*.h TauAnalysisAlgorithms/*.icc Root/*.cxx
   PUBLIC_HEADERS TauAnalysisAlgorithms
   LINK_LIBRARIES xAODTau SelectionHelpersLib
   SystematicsHandlesLib
   AnaAlgorithmLib TauAnalysisToolsLib )

atlas_add_dictionary( TauAnalysisAlgorithmsDict
   TauAnalysisAlgorithms/TauAnalysisAlgorithmsDict.h
   TauAnalysisAlgorithms/selection.xml
   LINK_LIBRARIES TauAnalysisAlgorithmsLib )

if( NOT XAOD_STANDALONE )
   atlas_add_component( TauAnalysisAlgorithms
      src/*.h src/*.cxx src/components/*.cxx
      LINK_LIBRARIES GaudiKernel TauAnalysisAlgorithmsLib )
endif()

atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*_jobOptions.py )
atlas_install_scripts( share/*_eljob.py )
atlas_install_data( data/*.conf )

if( XAOD_STANDALONE )
   atlas_add_test( testJobData
      SCRIPT TauAnalysisAlgorithmsTest_eljob.py --data-type data --unit-test
      PROPERTIES TIMEOUT 600 )
   # atlas_add_test( testJobFullSim    #testJobFullSim and testJobFastSim are temporarily removed awaiting TauTruthMatchigToolMigration (ATLASG-1548) 
   #    SCRIPT TauAnalysisAlgorithmsTest_eljob.py --data-type mc --unit-test
   #    PROPERTIES TIMEOUT 600 )
   # atlas_add_test( testJobFastSim
   #    SCRIPT TauAnalysisAlgorithmsTest_eljob.py --data-type afii --unit-test
   #    PROPERTIES TIMEOUT 600 )
else()
   atlas_add_test( testJobData
      SCRIPT athena.py
      TauAnalysisAlgorithms/TauAnalysisAlgorithmsTest_jobOptions.py - --data-type data
      PROPERTIES TIMEOUT 600 )
   atlas_add_test( testJobFullSim
      SCRIPT athena.py
      TauAnalysisAlgorithms/TauAnalysisAlgorithmsTest_jobOptions.py - --data-type mc
      PROPERTIES TIMEOUT 600 )
   atlas_add_test( testJobFastSim
      SCRIPT athena.py
      TauAnalysisAlgorithms/TauAnalysisAlgorithmsTest_jobOptions.py - --data-type afii
      PROPERTIES TIMEOUT 600 )
endif()
