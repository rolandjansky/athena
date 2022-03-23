# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigBunchCrossingTool )

# Component(s) in the package:
if( XAOD_STANDALONE )
   atlas_add_library( TrigBunchCrossingTool
      TrigBunchCrossingTool/*.h Root/*.h Root/*.cxx
      PUBLIC_HEADERS TrigBunchCrossingTool
      LINK_LIBRARIES AsgTools TrigAnalysisInterfaces
      PRIVATE_LINK_LIBRARIES xAODTrigger TrigConfL1Data TrigConfInterfaces )
else()
   atlas_add_library( TrigBunchCrossingToolLib
      TrigBunchCrossingTool/*.h
      INTERFACE
      PUBLIC_HEADERS TrigBunchCrossingTool
      LINK_LIBRARIES AsgTools TrigAnalysisInterfaces TrigConfInterfaces )

   atlas_add_component( TrigBunchCrossingTool
      TrigBunchCrossingTool/*.h src/*.cxx Root/*.h Root/*.cxx
      Root/json/*.h Root/json/*.inl
      src/components/*.cxx
      LINK_LIBRARIES AthenaKernel GaudiKernel TrigBunchCrossingToolLib TrigConfL1Data xAODTrigger )
endif()

atlas_add_test( ut_static_bunch_tool_test
   SOURCES
   test/ut_static_bunch_tool_test.cxx
   Root/BunchCrossing.cxx
   Root/BunchTrain.cxx
   Root/BunchCrossingToolBase.cxx
   Root/StaticBunchCrossingTool.cxx
   Root/count_bunch_neighbors.cxx
   LINK_LIBRARIES AsgTools xAODTrigger TrigConfL1Data TrigAnalysisInterfaces
   POST_EXEC_SCRIPT nopost.sh )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
