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
    Reconstruction/Jet/JetResolution
    Reconstruction/MET/METUtilities )

atlas_add_library( JetAnalysisAlgorithmsLib
   JetAnalysisAlgorithms/*.h JetAnalysisAlgorithms/*.icc Root/*.cxx
   PUBLIC_HEADERS JetAnalysisAlgorithms
   LINK_LIBRARIES xAODJet SelectionHelpersLib SystematicsHandlesLib
   AnaAlgorithmLib JetCalibToolsLib JetInterface JetResolutionLib
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
   atlas_add_test( testJobData
      SCRIPT JetAnalysisAlgorithmsTest_eljob.py --data-type data --unit-test
      PROPERTIES TIMEOUT 300 )
   atlas_add_test( testJobFullSim
      SCRIPT JetAnalysisAlgorithmsTest_eljob.py --data-type mc --unit-test
      PROPERTIES TIMEOUT 300 )
   atlas_add_test( testJobFastSim
      SCRIPT JetAnalysisAlgorithmsTest_eljob.py --data-type afii --unit-test
      PROPERTIES TIMEOUT 300 )
else()
   atlas_add_test( testJob
      SCRIPT athena.py
      JetAnalysisAlgorithms/JetAnalysisAlgorithmsTest_jobOptions.py
      PROPERTIES TIMEOUT 300 )
endif()
