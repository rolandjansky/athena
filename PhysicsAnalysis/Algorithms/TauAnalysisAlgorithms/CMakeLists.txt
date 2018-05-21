# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
#
# @author Nils Krumnack


# The name of the package:
atlas_subdir( TauAnalysisAlgorithms )

# The package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   Event/xAOD/xAODTau
   PhysicsAnalysis/Algorithms/SelectionHelpers
   PhysicsAnalysis/Algorithms/SystematicsHandles
   PhysicsAnalysis/D3PDTools/AnaAlgorithm
   PhysicsAnalysis/TauID/TauAnalysisTools )

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

# only running test in AnalysisBase for now (the Athena sequence
# doesn't fully work yet).
if( XAOD_STANDALONE )
   atlas_add_test( testJob
      SCRIPT TauAnalysisAlgorithmsTest_eljob.py )
#else()
#   atlas_add_test( testJob
#      SCRIPT athena.py
#      TauAnalysisAlgorithms/TauAnalysisAlgorithmsTest_jobOptions.py )
endif()
