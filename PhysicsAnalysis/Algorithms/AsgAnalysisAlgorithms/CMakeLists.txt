# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
#
# @author Nils Krumnack


atlas_subdir( AsgAnalysisAlgorithms )

atlas_depends_on_subdirs(
   PUBLIC
   Event/xAOD/xAODBase
   PhysicsAnalysis/Algorithms/SelectionHelpers
   PhysicsAnalysis/Algorithms/SystematicsHandles
   PhysicsAnalysis/AnalysisCommon/PATCore
   PhysicsAnalysis/AnalysisCommon/PATInterfaces
   PhysicsAnalysis/D3PDTools/AnaAlgorithm
   PhysicsAnalysis/Interfaces/AsgAnalysisInterfaces
   PRIVATE
   Control/AthToolSupport/AsgTools
   Event/xAOD/xAODJet
   Event/xAOD/xAODMuon
   Event/xAOD/xAODEgamma
   PhysicsAnalysis/D3PDTools/RootCoreUtils )

atlas_add_library( AsgAnalysisAlgorithmsLib
   AsgAnalysisAlgorithms/*.h AsgAnalysisAlgorithms/*.icc Root/*.cxx
   PUBLIC_HEADERS AsgAnalysisAlgorithms
   LINK_LIBRARIES xAODBase SelectionHelpersLib SystematicsHandlesLib
     PATInterfaces AnaAlgorithmLib AsgAnalysisInterfaces PATCoreLib
   PRIVATE_LINK_LIBRARIES AsgTools RootCoreUtils
     xAODJet xAODMuon xAODEgamma )

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
