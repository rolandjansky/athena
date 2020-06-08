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
   atlas_add_component( TrigBunchCrossingTool
      TrigBunchCrossingTool/*.h src/*.cxx Root/*.h Root/*.cxx
      Root/json/*.h Root/json/*.inl
      src/components/*.cxx
      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS}
      LINK_LIBRARIES ${ROOT_LIBRARIES} ${CORAL_LIBRARIES} AsgTools
      TrigAnalysisInterfaces TrigConfInterfaces GaudiKernel AthenaKernel
      AthenaPoolUtilities xAODEventInfo xAODTrigger TrigConfL1Data )
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

atlas_add_test( ut_web_bunch_tool_test
   SOURCES
   test/ut_web_bunch_tool_test.cxx
   Root/BunchCrossing.cxx
   Root/BunchTrain.cxx
   Root/BunchCrossingToolBase.cxx
   Root/WebBunchCrossingTool.cxx
   Root/count_bunch_neighbors.cxx
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} AsgTools xAODEventInfo xAODTrigger
   TrigConfL1Data TrigAnalysisInterfaces
   PROPERTIES TIMEOUT 300 )

# Install files from the package:

# temporarily disabling FLAKE8 in AnalysisBase until it becomes
# available there.
if( XAOD_STANDALONE )
atlas_install_python_modules( python/*.py )
else()
atlas_install_python_modules( python/*.py  POST_BUILD_CMD ${ATLAS_FLAKE8} )

atlas_add_test( flake8_scripts
                SCRIPT flake8 --select=ATL,F,E7,E9,W6 ${CMAKE_CURRENT_SOURCE_DIR}/scripts
                POST_EXEC_SCRIPT nopost.sh )
endif()
