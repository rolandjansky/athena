# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
#
# @author Nils Krumnack


# The name of the package:
atlas_subdir( EgammaAnalysisAlgorithms )

# The package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   Event/xAOD/xAODEgamma
   PhysicsAnalysis/Algorithms/SelectionHelpers
   PhysicsAnalysis/Algorithms/SystematicsHandles
   PhysicsAnalysis/AnalysisCommon/IsolationSelection
   PhysicsAnalysis/D3PDTools/AnaAlgorithm
   PhysicsAnalysis/ElectronPhotonID/IsolationCorrections
   PhysicsAnalysis/Interfaces/EgammaAnalysisInterfaces )

atlas_add_library( EgammaAnalysisAlgorithmsLib
   EgammaAnalysisAlgorithms/*.h EgammaAnalysisAlgorithms/*.icc Root/*.cxx
   PUBLIC_HEADERS EgammaAnalysisAlgorithms
   LINK_LIBRARIES xAODEgamma SelectionHelpersLib SystematicsHandlesLib
   IsolationSelectionLib AnaAlgorithmLib IsolationCorrectionsLib
   EgammaAnalysisInterfacesLib )

atlas_add_dictionary( EgammaAnalysisAlgorithmsDict
   EgammaAnalysisAlgorithms/EgammaAnalysisAlgorithmsDict.h
   EgammaAnalysisAlgorithms/selection.xml
   LINK_LIBRARIES EgammaAnalysisAlgorithmsLib )

if( NOT XAOD_STANDALONE )
   atlas_add_component( EgammaAnalysisAlgorithms
      src/*.h src/*.cxx src/components/*.cxx
      LINK_LIBRARIES GaudiKernel EgammaAnalysisAlgorithmsLib )
endif()

atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*_jobOptions.py )
atlas_install_scripts( share/*_eljob.py )

if( XAOD_STANDALONE )
   # temporarily disable these until AnalysisBase is fully functional

   # atlas_add_test( testJobData
   #    SCRIPT EgammaAnalysisAlgorithmsTest_eljob.py --data-type data --unit-test
   #    PROPERTIES TIMEOUT 600 )
   # atlas_add_test( testJobFullSim
   #    SCRIPT EgammaAnalysisAlgorithmsTest_eljob.py --data-type mc --unit-test
   #    PROPERTIES TIMEOUT 600 )
   # atlas_add_test( testJobFastSim
   #    SCRIPT EgammaAnalysisAlgorithmsTest_eljob.py --data-type afii --unit-test
   #    PROPERTIES TIMEOUT 600 )
else()
   atlas_add_test( testJobData
      SCRIPT athena.py
      EgammaAnalysisAlgorithms/EgammaAnalysisAlgorithmsTest_jobOptions.py - --data-type data
      PROPERTIES TIMEOUT 600 )
   atlas_add_test( testJobFullSim
      SCRIPT athena.py
      EgammaAnalysisAlgorithms/EgammaAnalysisAlgorithmsTest_jobOptions.py - --data-type mc
      PROPERTIES TIMEOUT 600 )
   atlas_add_test( testJobFastSim
      SCRIPT athena.py
      EgammaAnalysisAlgorithms/EgammaAnalysisAlgorithmsTest_jobOptions.py - --data-type afii
      PROPERTIES TIMEOUT 600 )
endif()
