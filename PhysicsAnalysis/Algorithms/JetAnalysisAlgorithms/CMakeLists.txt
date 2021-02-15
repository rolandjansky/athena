# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# The name of the package:
atlas_subdir( JetAnalysisAlgorithms )

atlas_add_library( JetAnalysisAlgorithmsLib
   JetAnalysisAlgorithms/*.h JetAnalysisAlgorithms/*.icc Root/*.cxx
   PUBLIC_HEADERS JetAnalysisAlgorithms
   LINK_LIBRARIES xAODJet SelectionHelpersLib SystematicsHandlesLib
   AnaAlgorithmLib JetCalibToolsLib JetInterface
   JetCPInterfaces JetAnalysisInterfacesLib
   PRIVATE_LINK_LIBRARIES METUtilitiesLib xAODMuon )

atlas_add_dictionary( JetAnalysisAlgorithmsDict
   JetAnalysisAlgorithms/JetAnalysisAlgorithmsDict.h
   JetAnalysisAlgorithms/selection.xml
   LINK_LIBRARIES JetAnalysisAlgorithmsLib )

if( NOT XAOD_STANDALONE )
   atlas_add_component( JetAnalysisAlgorithms
      src/*.h src/*.cxx src/components/*.cxx
      LINK_LIBRARIES AnaAlgorithmLib JetAnalysisInterfacesLib JetCPInterfaces JetCalibToolsLib
      JetInterface SelectionHelpersLib SystematicsHandlesLib xAODJet JetAnalysisAlgorithmsLib )
endif()

atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/*_jobOptions.py )
atlas_install_scripts( share/*_eljob.py )

if( XAOD_STANDALONE )
  # atlas_add_test( testJobDataEMTopo
  #    SCRIPT JetAnalysisAlgorithmsTest_EMTopo_eljob.py --data-type data --unit-test
  #    PROPERTIES TIMEOUT 600 )
  atlas_add_test( testJobDataPFlow
     SCRIPT JetAnalysisAlgorithmsTest_PFlow_eljob.py --data-type data --unit-test
     PROPERTIES TIMEOUT 600 )
  # atlas_add_test( testJobFullSimEMTopo
  #    SCRIPT JetAnalysisAlgorithmsTest_EMTopo_eljob.py --data-type mc --unit-test
  #    PROPERTIES TIMEOUT 600 )
  atlas_add_test( testJobFullSimPFlow
     SCRIPT JetAnalysisAlgorithmsTest_PFlow_eljob.py --data-type mc --unit-test
     PROPERTIES TIMEOUT 600 )
  # atlas_add_test( testJobFastSimEMTopo
  #    SCRIPT JetAnalysisAlgorithmsTest_EMTopo_eljob.py --data-type afii --unit-test
  #    PROPERTIES TIMEOUT 600 )
  atlas_add_test( testJobFastSimPFlow
     SCRIPT JetAnalysisAlgorithmsTest_PFlow_eljob.py --data-type afii --unit-test
     PROPERTIES TIMEOUT 600 )
else()
   # atlas_add_test( testJobDataEMTopo
   #    SCRIPT athena.py
   #    JetAnalysisAlgorithms/JetAnalysisAlgorithmsTest_EMTopo_jobOptions.py - --data-type data
   #    PROPERTIES TIMEOUT 600 )
   atlas_add_test( testJobDataPFlow
      SCRIPT athena.py
      JetAnalysisAlgorithms/JetAnalysisAlgorithmsTest_PFlow_jobOptions.py - --data-type data
      PROPERTIES TIMEOUT 600 )
   # atlas_add_test( testJobFullSimEMTopo
   #    SCRIPT athena.py
   #    JetAnalysisAlgorithms/JetAnalysisAlgorithmsTest_EMTopo_jobOptions.py - --data-type mc
   #    PROPERTIES TIMEOUT 600 )
   atlas_add_test( testJobFullSimPFlow
      SCRIPT athena.py
      JetAnalysisAlgorithms/JetAnalysisAlgorithmsTest_PFlow_jobOptions.py - --data-type mc
      PROPERTIES TIMEOUT 600 )
   # atlas_add_test( testJobFastSimEMTopo
   #    SCRIPT athena.py
   #    JetAnalysisAlgorithms/JetAnalysisAlgorithmsTest_EMTopo_jobOptions.py - --data-type afii
   #    PROPERTIES TIMEOUT 600 )
   atlas_add_test( testJobFastSimPFlow
      SCRIPT athena.py
      JetAnalysisAlgorithms/JetAnalysisAlgorithmsTest_PFlow_jobOptions.py - --data-type afii
      PROPERTIES TIMEOUT 600 )
endif()
