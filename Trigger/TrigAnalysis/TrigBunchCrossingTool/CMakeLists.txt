# $Id: CMakeLists.txt 782843 2016-11-07 20:39:27Z krasznaa $
################################################################################
# Package: TrigBunchCrossingTool
################################################################################

# Declare the package name:
atlas_subdir( TrigBunchCrossingTool )

# Declare the package's dependencies:
if( XAOD_STANDALONE )
   atlas_depends_on_subdirs(
      PUBLIC
      Control/AthToolSupport/AsgTools
      Trigger/TrigAnalysis/TrigAnalysisInterfaces
      PRIVATE
      Control/xAODRootAccess
      Event/xAOD/xAODEventInfo
      Event/xAOD/xAODTrigger
      Trigger/TrigConfiguration/TrigConfInterfaces
      Trigger/TrigConfiguration/TrigConfL1Data )
else()
   atlas_depends_on_subdirs(
      PUBLIC
      Control/AthToolSupport/AsgTools
      GaudiKernel
      Trigger/TrigAnalysis/TrigAnalysisInterfaces
      PRIVATE
      Control/AthenaKernel
      Database/AthenaPOOL/AthenaPoolUtilities
      Event/xAOD/xAODEventInfo
      Event/xAOD/xAODTrigger
      Trigger/TrigConfiguration/TrigConfInterfaces
      Trigger/TrigConfiguration/TrigConfL1Data )
endif()

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
      xAODTrigger TrigConfL1Data )
else()
   atlas_add_component( TrigBunchCrossingTool
      TrigBunchCrossingTool/*.h src/*.cxx Root/*.h Root/*.cxx
      Root/json/*.h Root/json/*.inl
      src/components/*.cxx
      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS}
      LINK_LIBRARIES ${ROOT_LIBRARIES} ${CORAL_LIBRARIES} AsgTools
      TrigAnalysisInterfaces GaudiKernel AthenaKernel
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
   TrigConfL1Data )

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
   TrigConfL1Data )

if( XAOD_STANDALONE )

   atlas_add_test( ut_trigconf_bunch_tool_test
      SOURCES test/ut_trigconf_bunch_tool_test.cxx
      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
      LINK_LIBRARIES ${ROOT_LIBRARIES} xAODRootAccess TrigBunchCrossingTool )

   atlas_add_test( ut_web_xaod_bunch_tool_test
      SOURCES test/ut_web_xaod_bunch_tool_test.cxx
      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
      LINK_LIBRARIES ${ROOT_LIBRARIES} xAODRootAccess TrigBunchCrossingTool )

endif()

# Install files from the package:
atlas_install_headers( TrigBunchCrossingTool )
atlas_install_python_modules( python/*.py )

