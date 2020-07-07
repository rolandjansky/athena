# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
#
# @author Nils Krumnack


# The name of the package:
atlas_subdir( JetAnalysisAlgorithms )

atlas_depends_on_subdirs(
    PUBLIC
    Event/xAOD/xAODJet
    PhysicsAnalysis/Algorithms/SelectionHelpers
    PhysicsAnalysis/Algorithms/SystematicsHandles
    PhysicsAnalysis/D3PDTools/AnaAlgorithm
    Reconstruction/Jet/JetCalibTools
    Reconstruction/Jet/JetCPInterfaces
    Reconstruction/Jet/JetInterface
    Reconstruction/Jet/JetJvtEfficiency
    Reconstruction/MET/METUtilities )

atlas_add_library( JetAnalysisAlgorithmsLib
   JetAnalysisAlgorithms/*.h JetAnalysisAlgorithms/*.icc Root/*.cxx
   PUBLIC_HEADERS JetAnalysisAlgorithms
   LINK_LIBRARIES xAODJet SelectionHelpersLib SystematicsHandlesLib
   AnaAlgorithmLib JetCalibToolsLib JetInterface
   JetCPInterfaces JetJvtEfficiencyLib JetAnalysisInterfacesLib METUtilitiesLib )

atlas_add_dictionary( JetAnalysisAlgorithmsDict
   JetAnalysisAlgorithms/JetAnalysisAlgorithmsDict.h
   JetAnalysisAlgorithms/selection.xml
   LINK_LIBRARIES JetAnalysisAlgorithmsLib )

if( NOT XAOD_STANDALONE )
   atlas_add_component( JetAnalysisAlgorithms
      src/*.h src/*.cxx src/components/*.cxx
      LINK_LIBRARIES GaudiKernel JetAnalysisAlgorithmsLib )
endif()

atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*_jobOptions.py )
atlas_install_scripts( share/*_eljob.py )

if( XAOD_STANDALONE )
   atlas_add_test( testJobDataEMTopo
      SCRIPT JetAnalysisAlgorithmsTest_EMTopo_eljob.py --data-type data --unit-test
      PROPERTIES TIMEOUT 600 )
   atlas_add_test( testJobDataPFlow
      SCRIPT JetAnalysisAlgorithmsTest_PFlow_eljob.py --data-type data --unit-test
      PROPERTIES TIMEOUT 600 )
   atlas_add_test( testJobFullSimEMTopo
      SCRIPT JetAnalysisAlgorithmsTest_EMTopo_eljob.py --data-type mc --unit-test
      PROPERTIES TIMEOUT 600 )
   atlas_add_test( testJobFullSimPFlow
      SCRIPT JetAnalysisAlgorithmsTest_PFlow_eljob.py --data-type mc --unit-test
      PROPERTIES TIMEOUT 600 )
   atlas_add_test( testJobFastSimEMTopo
      SCRIPT JetAnalysisAlgorithmsTest_EMTopo_eljob.py --data-type afii --unit-test
      PROPERTIES TIMEOUT 600 )
   atlas_add_test( testJobFastSimPFlow
      SCRIPT JetAnalysisAlgorithmsTest_PFlow_eljob.py --data-type afii --unit-test
      PROPERTIES TIMEOUT 600 )
else()
   atlas_add_test( testJobDataEMTopo
      SCRIPT athena.py
      JetAnalysisAlgorithms/JetAnalysisAlgorithmsTest_EMTopo_jobOptions.py - --data-type data
      PROPERTIES TIMEOUT 600 )
   atlas_add_test( testJobDataPFlow
      SCRIPT athena.py
      JetAnalysisAlgorithms/JetAnalysisAlgorithmsTest_PFlow_jobOptions.py - --data-type data
      PROPERTIES TIMEOUT 600 )
   atlas_add_test( testJobFullSimEMTopo
      SCRIPT athena.py
      JetAnalysisAlgorithms/JetAnalysisAlgorithmsTest_EMTopo_jobOptions.py - --data-type mc
      PROPERTIES TIMEOUT 600 )
   atlas_add_test( testJobFullSimPFlow
      SCRIPT athena.py
      JetAnalysisAlgorithms/JetAnalysisAlgorithmsTest_PFlow_jobOptions.py - --data-type mc
      PROPERTIES TIMEOUT 600 )
   atlas_add_test( testJobFastSimEMTopo
      SCRIPT athena.py
      JetAnalysisAlgorithms/JetAnalysisAlgorithmsTest_EMTopo_jobOptions.py - --data-type afii
      PROPERTIES TIMEOUT 600 )
   atlas_add_test( testJobFastSimPFlow
      SCRIPT athena.py
      JetAnalysisAlgorithms/JetAnalysisAlgorithmsTest_PFlow_jobOptions.py - --data-type afii
      PROPERTIES TIMEOUT 600 )
endif()
