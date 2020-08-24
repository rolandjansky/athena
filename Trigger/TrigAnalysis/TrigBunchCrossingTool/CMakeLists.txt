# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigBunchCrossingTool )

# External dependencies:
find_package( CORAL QUIET COMPONENTS CoralBase CoralKernel RelationalAccess )
find_package( ROOT COMPONENTS Net Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
if( XAOD_STANDALONE )
   atlas_add_library( TrigBunchCrossingTool
      TrigBunchCrossingTool/*.h Root/*.h Root/*.cxx
      Root/json/*.h Root/json/*.inl
      PUBLIC_HEADERS TrigBunchCrossingTool
      LINK_LIBRARIES AsgTools TrigAnalysisInterfaces
      PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
      PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} xAODEventInfo
      xAODTrigger TrigConfL1Data TrigConfInterfaces )
else()
   atlas_add_library( TrigBunchCrossingToolLib
      TrigBunchCrossingTool/*.h
      INTERFACE
      PUBLIC_HEADERS TrigBunchCrossingTool
      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
      LINK_LIBRARIES ${ROOT_LIBRARIES} AsgTools TrigAnalysisInterfaces TrigConfInterfaces )

   atlas_add_component( TrigBunchCrossingTool
      TrigBunchCrossingTool/*.h src/*.cxx Root/*.h Root/*.cxx
      Root/json/*.h Root/json/*.inl
      src/components/*.cxx
      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS}
      LINK_LIBRARIES ${CORAL_LIBRARIES} AthenaKernel AthenaPoolUtilities CxxUtils
      GaudiKernel TrigBunchCrossingToolLib TrigConfL1Data xAODEventInfo xAODTrigger )
endif()

atlas_add_test( ut_static_bunch_tool_test
   SOURCES
   test/ut_static_bunch_tool_test.cxx
   Root/BunchCrossing.cxx
   Root/BunchTrain.cxx
   Root/BunchCrossingToolBase.cxx
   Root/StaticBunchCrossingTool.cxx
   Root/count_bunch_neighbors.cxx
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} AsgTools xAODEventInfo xAODTrigger
   TrigConfL1Data TrigAnalysisInterfaces )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
