# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( EGammaVariableCorrection )

# External dependencies:
find_package( ROOT COMPONENTS Core MathCore Hist Graph RIO )

# Component(s) in the package:
atlas_add_library( EGammaVariableCorrectionLib
   EGammaVariableCorrection/*.h
   Root/*.cxx
   PUBLIC_HEADERS EGammaVariableCorrection
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} AsgTools EgammaAnalysisInterfacesLib EgammaAnalysisHelpersLib 
   PRIVATE_LINK_LIBRARIES PathResolver xAODEgamma xAODEventShape)

if( NOT XAOD_STANDALONE )
   atlas_add_component( EGammaVariableCorrection
      src/components/*.cxx
      LINK_LIBRARIES EGammaVariableCorrectionLib )
endif()

atlas_add_dictionary( ElectronPhotonVariableCorrectionDict
   EGammaVariableCorrection/ElectronPhotonVariableCorrectionDict.h
   EGammaVariableCorrection/selection.xml
   LINK_LIBRARIES EGammaVariableCorrectionLib )

# Add tests
if( XAOD_STANDALONE )
   atlas_add_executable( Test_ElectronPhotonVariableCorrectionBase
   util/testElectronPhotonVariableCorrectionBase.cxx
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} EGammaVariableCorrectionLib)

   atlas_add_executable( Test_IsoCorrection
   util/testIsoCorrection.cxx
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} IsolationCorrectionsLib EGammaVariableCorrectionLib)

   atlas_add_executable( Test_ElectronPhotonVariableCorrectionTool
   util/testElectronPhotonVariableCorrectionTool.cxx
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} EGammaVariableCorrectionLib)
endif()

atlas_add_executable( Test_ElectronPhotonVariableCorrectionTool_DictionaryToolHandle
   util/testElectronPhotonVariableCorrectionTool_DictionaryToolHandle.cxx
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} EGammaVariableCorrectionLib)

# Install files from the package:
atlas_install_data( data/*.conf )
