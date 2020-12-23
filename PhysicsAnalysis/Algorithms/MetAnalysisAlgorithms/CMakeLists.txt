# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# The name of the package:
atlas_subdir( MetAnalysisAlgorithms )

atlas_add_library( MetAnalysisAlgorithmsLib
   MetAnalysisAlgorithms/*.h MetAnalysisAlgorithms/*.icc Root/*.cxx
   PUBLIC_HEADERS MetAnalysisAlgorithms
   LINK_LIBRARIES AnaAlgorithmLib METInterface SystematicsHandlesLib xAODBase xAODMissingET
   PRIVATE_LINK_LIBRARIES METUtilitiesLib xAODEventInfo )

atlas_add_dictionary( MetAnalysisAlgorithmsDict
   MetAnalysisAlgorithms/MetAnalysisAlgorithmsDict.h
   MetAnalysisAlgorithms/selection.xml
   LINK_LIBRARIES MetAnalysisAlgorithmsLib )

if( NOT XAOD_STANDALONE )
   atlas_add_component( MetAnalysisAlgorithms
      src/*.h src/*.cxx src/components/*.cxx
      LINK_LIBRARIES AnaAlgorithmLib METInterface SystematicsHandlesLib xAODBase
      xAODMissingET MetAnalysisAlgorithmsLib )
endif()

atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/*_jobOptions.py )
atlas_install_scripts( share/*_eljob.py )

if( XAOD_STANDALONE )
   atlas_add_test( testJobData
      SCRIPT MetAnalysisAlgorithmsTest_eljob.py --data-type data --unit-test
      PROPERTIES TIMEOUT 600 )
   atlas_add_test( testJobFullSim
      SCRIPT MetAnalysisAlgorithmsTest_eljob.py --data-type mc --unit-test
      PROPERTIES TIMEOUT 600 )
   atlas_add_test( testJobFastSim
      SCRIPT MetAnalysisAlgorithmsTest_eljob.py --data-type afii --unit-test
      PROPERTIES TIMEOUT 600 )
elseif( NOT "${CMAKE_PROJECT_NAME}" STREQUAL "AthDerivation" )
   atlas_add_test( testJobData
      SCRIPT athena.py
      MetAnalysisAlgorithms/MetAnalysisAlgorithmsTest_jobOptions.py - --data-type data
      PROPERTIES TIMEOUT 600 )
   atlas_add_test( testJobFullSim
      SCRIPT athena.py
      MetAnalysisAlgorithms/MetAnalysisAlgorithmsTest_jobOptions.py - --data-type mc
      PROPERTIES TIMEOUT 600 )
   atlas_add_test( testJobFastSim
      SCRIPT athena.py
      MetAnalysisAlgorithms/MetAnalysisAlgorithmsTest_jobOptions.py - --data-type afii
      PROPERTIES TIMEOUT 600 )
endif()
