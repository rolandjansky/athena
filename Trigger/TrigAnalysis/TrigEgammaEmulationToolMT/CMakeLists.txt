# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigEgammaEmulationToolMT )

# External dependencies:
find_package( Boost )
find_package( CLHEP )

# Component(s) in the package:
atlas_add_library( TrigEgammaEmulationToolMTLib
   Root/*.cxx
   PUBLIC_HEADERS TrigEgammaEmulationToolMT
   PRIVATE_INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
   PRIVATE_LINK_LIBRARIES ${Boost_LIBRARIES} TrigSteeringEvent xAODRootAccess
   LINK_LIBRARIES AsgTools AthContainers 
   EgammaAnalysisInterfacesLib InDetTrackSelectionToolLib 
   LumiBlockCompsLib PATCoreLib RecoToolInterfaces 
   TrigNavStructure TrigDecisionToolLib TrigEgammaMatchingToolLib 
   xAODBase xAODCaloEvent xAODEgamma xAODTracking xAODTrigCalo 
   xAODTrigRinger xAODTrigger RingerSelectorToolsLib
   AthOnnxruntimeServiceLib
    )

atlas_add_component( TrigEgammaEmulationToolMT
   src/*.cxx 
   src/components/*.cxx
   LINK_LIBRARIES AthenaBaseComps GaudiKernel TrigEgammaEmulationToolMTLib )

# Install files from the package:
atlas_install_python_modules( python/TrigEgamma*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
# atlas_install_joboptions( share/test*.py )
