# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
#
# @author Nils Krumnack


# The name of the package:
atlas_subdir( MuonAnalysisAlgorithms )

# The package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   PhysicsAnalysis/D3PDTools/AnaAlgorithm
   Control/AthToolSupport/AsgTools )

atlas_add_library( MuonAnalysisAlgorithmsLib
   MuonAnalysisAlgorithms/*.h MuonAnalysisAlgorithms/*.icc Root/*.cxx
   PUBLIC_HEADERS MuonAnalysisAlgorithms
   LINK_LIBRARIES SelectionHelpersLib SystematicsHandlesLib
   MuonAnalysisInterfacesLib IsolationSelectionLib
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
