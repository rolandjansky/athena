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
