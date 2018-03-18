# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
#
# @author Nils Krumnack


atlas_subdir( AsgAnalysisAlgorithms )

atlas_depends_on_subdirs(
   PUBLIC
   Event/xAOD/xAODBase
   PhysicsAnalysis/Algorithms/SelectionHelpers
   PhysicsAnalysis/Algorithms/SystematicsHandles
   PhysicsAnalysis/AnalysisCommon/PATInterfaces
   PhysicsAnalysis/D3PDTools/AnaAlgorithm
   PRIVATE
   Control/AthToolSupport/AsgTools
   PhysicsAnalysis/D3PDTools/RootCoreUtils )

atlas_add_library( AsgAnalysisAlgorithmsLib
   AsgAnalysisAlgorithms/*.h AsgAnalysisAlgorithms/*.icc Root/*.cxx
   PUBLIC_HEADERS AsgAnalysisAlgorithms
   LINK_LIBRARIES  SelectionHelpersLib SystematicsHandlesLib
     PATInterfaces AnaAlgorithmLib xAODBase
   PRIVATE_LINK_LIBRARIES AsgTools RootCoreUtils )

atlas_add_dictionary( AsgAnalysisAlgorithmsDict
   AsgAnalysisAlgorithms/AsgAnalysisAlgorithmsDict.h
   AsgAnalysisAlgorithms/selection.xml
   LINK_LIBRARIES AsgAnalysisAlgorithmsLib )

if( NOT XAOD_STANDALONE )
   atlas_add_component( AsgAnalysisAlgorithms
      src/*.h src/*.cxx src/components/*.cxx
      LINK_LIBRARIES GaudiKernel AsgAnalysisAlgorithmsLib )
endif()

atlas_install_python_modules( python/*.py )
