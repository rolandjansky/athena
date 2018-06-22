# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
#
# @author Nils Krumnack


# The name of the package:
atlas_subdir( MetAnalysisAlgorithms )

# The package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   Event/xAOD/xAODMissingET
   PhysicsAnalysis/Algorithms/SystematicsHandles
   PhysicsAnalysis/D3PDTools/AnaAlgorithm
   PhysicsAnalysis/Interfaces/METInterface
   Reconstruction/MET/METInterface
   PRIVATE )

atlas_add_library( MetAnalysisAlgorithmsLib
   MetAnalysisAlgorithms/*.h MetAnalysisAlgorithms/*.icc Root/*.cxx
   PUBLIC_HEADERS MetAnalysisAlgorithms
   LINK_LIBRARIES xAODMissingET
   SystematicsHandlesLib AnaAlgorithmLib METInterface
   SelectionHelpersLib
   PRIVATE_LINK_LIBRARIES  )

atlas_add_dictionary( MetAnalysisAlgorithmsDict
   MetAnalysisAlgorithms/MetAnalysisAlgorithmsDict.h
   MetAnalysisAlgorithms/selection.xml
   LINK_LIBRARIES MetAnalysisAlgorithmsLib )

if( NOT XAOD_STANDALONE )
   atlas_add_component( MetAnalysisAlgorithms
      src/*.h src/*.cxx src/components/*.cxx
      LINK_LIBRARIES GaudiKernel MetAnalysisAlgorithmsLib )
endif()

atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*_jobOptions.py )
atlas_install_scripts( share/*_eljob.py )

if( XAOD_STANDALONE )
   atlas_add_test( testJob
      SCRIPT MetAnalysisAlgorithmsTest_eljob.py )
else()
   atlas_add_test( testJob
      SCRIPT athena.py
      MetAnalysisAlgorithms/MetAnalysisAlgorithmsTest_jobOptions.py )
endif()
