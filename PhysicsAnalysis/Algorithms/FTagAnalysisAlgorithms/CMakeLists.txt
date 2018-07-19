# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
#
# @author Nils Krumnack


# The name of the package:
atlas_subdir( FTagAnalysisAlgorithms )

atlas_depends_on_subdirs(
    PUBLIC
    Event/xAOD/xAODJet
    PhysicsAnalysis/Algorithms/SelectionHelpers
    PhysicsAnalysis/Algorithms/SystematicsHandles
    PhysicsAnalysis/D3PDTools/AnaAlgorithm
    PhysicsAnalysis/Interfaces/FTagAnalysisInterfaces )

atlas_add_library( FTagAnalysisAlgorithmsLib
   FTagAnalysisAlgorithms/*.h FTagAnalysisAlgorithms/*.icc Root/*.cxx
   PUBLIC_HEADERS FTagAnalysisAlgorithms
   LINK_LIBRARIES xAODJet SelectionHelpersLib SystematicsHandlesLib
   AnaAlgorithmLib FTagAnalysisInterfacesLib )

atlas_add_dictionary( FTagAnalysisAlgorithmsDict
   FTagAnalysisAlgorithms/FTagAnalysisAlgorithmsDict.h
   FTagAnalysisAlgorithms/selection.xml
   LINK_LIBRARIES FTagAnalysisAlgorithmsLib )

if( NOT XAOD_STANDALONE )
   atlas_add_component( FTagAnalysisAlgorithms
      src/*.h src/*.cxx src/components/*.cxx
      LINK_LIBRARIES GaudiKernel FTagAnalysisAlgorithmsLib )
endif()

atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*_jobOptions.py )
atlas_install_scripts( share/*_eljob.py )

if( XAOD_STANDALONE )
   atlas_add_test( testJob
      SCRIPT FTagAnalysisAlgorithmsTest_eljob.py --unit-test )
else()
   atlas_add_test( testJob
      SCRIPT athena.py
      FTagAnalysisAlgorithms/FTagAnalysisAlgorithmsTest_jobOptions.py )
endif()
