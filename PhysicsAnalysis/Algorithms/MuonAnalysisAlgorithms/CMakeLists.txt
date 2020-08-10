# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
#
# @author Nils Krumnack


# The name of the package:
atlas_subdir( MuonAnalysisAlgorithms )

# The package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   Event/xAOD/xAODEventInfo
   Event/xAOD/xAODMuon
   PhysicsAnalysis/Algorithms/SelectionHelpers
   PhysicsAnalysis/Algorithms/SystematicsHandles
   PhysicsAnalysis/AnalysisCommon/IsolationSelection
   PhysicsAnalysis/D3PDTools/AnaAlgorithm
   PhysicsAnalysis/Interfaces/MuonAnalysisInterfaces
   PRIVATE
   PhysicsAnalysis/D3PDTools/RootCoreUtils )

atlas_add_library( MuonAnalysisAlgorithmsLib
   MuonAnalysisAlgorithms/*.h MuonAnalysisAlgorithms/*.icc Root/*.cxx
   PUBLIC_HEADERS MuonAnalysisAlgorithms
   LINK_LIBRARIES xAODEventInfo xAODMuon SelectionHelpersLib
   SystematicsHandlesLib MuonAnalysisInterfacesLib IsolationSelectionLib
   AnaAlgorithmLib
   PRIVATE_LINK_LIBRARIES RootCoreUtils )

atlas_add_dictionary( MuonAnalysisAlgorithmsDict
   MuonAnalysisAlgorithms/MuonAnalysisAlgorithmsDict.h
   MuonAnalysisAlgorithms/selection.xml
   LINK_LIBRARIES MuonAnalysisAlgorithmsLib )

if( NOT XAOD_STANDALONE )
   atlas_add_component( MuonAnalysisAlgorithms
      src/*.h src/*.cxx src/components/*.cxx
      LINK_LIBRARIES GaudiKernel MuonAnalysisAlgorithmsLib )
endif()

atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*_jobOptions.py )
atlas_install_scripts( share/*_eljob.py )

if( XAOD_STANDALONE )
   # temporarily disable these until AnalysisBase is fully functional

   # atlas_add_test( testJobData
   #    SCRIPT MuonAnalysisAlgorithmsTest_eljob.py --data-type data --unit-test
   #    PROPERTIES TIMEOUT 600 )
   # atlas_add_test( testJobFullSim
   #    SCRIPT MuonAnalysisAlgorithmsTest_eljob.py --data-type mc --unit-test
   #    PROPERTIES TIMEOUT 600 )
   # atlas_add_test( testJobFastSim
   #    SCRIPT MuonAnalysisAlgorithmsTest_eljob.py --data-type afii --unit-test
   #    PROPERTIES TIMEOUT 600 )
else()
   atlas_add_test( testJobData
      SCRIPT athena.py
      MuonAnalysisAlgorithms/MuonAnalysisAlgorithmsTest_jobOptions.py --evtMax=500 - --data-type data
      PROPERTIES TIMEOUT 600 )
   atlas_add_test( testJobFullSim
      SCRIPT athena.py
      MuonAnalysisAlgorithms/MuonAnalysisAlgorithmsTest_jobOptions.py --evtMax=500 - --data-type mc
      PROPERTIES TIMEOUT 600 )
   atlas_add_test( testJobFastSim
      SCRIPT athena.py
      MuonAnalysisAlgorithms/MuonAnalysisAlgorithmsTest_jobOptions.py --evtMax=500 - --data-type afii
      PROPERTIES TIMEOUT 600 )
endif()
