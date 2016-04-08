################################################################################
# Package: TrigBunchCrossingTool
################################################################################

# Declare the package name:
atlas_subdir( TrigBunchCrossingTool )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthToolSupport/AsgTools
                          GaudiKernel
                          Trigger/TrigAnalysis/TrigAnalysisInterfaces
                          PRIVATE
                          AtlasTest/TestTools
                          Control/AthenaKernel
                          Control/CxxUtils
                          Database/AthenaPOOL/AthenaPoolUtilities
                          Event/xAOD/xAODEventInfo
                          Event/xAOD/xAODTrigger
                          Trigger/TrigConfiguration/TrigConfInterfaces
                          Trigger/TrigConfiguration/TrigConfL1Data )

# External dependencies:
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )
find_package( ROOT COMPONENTS Net Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( TrigBunchCrossingTool
                     src/*.cxx
                     Root/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${CORAL_LIBRARIES} AsgTools GaudiKernel TestTools AthenaKernel CxxUtils AthenaPoolUtilities xAODEventInfo xAODTrigger TrigConfL1Data )

atlas_add_test( ut_static_bunch_tool_test
                SOURCES
                test/ut_static_bunch_tool_test.cxx
                Root/BunchCrossing.cxx
                Root/BunchTrain.cxx
                Root/BunchCrossingToolBase.cxx
                Root/StaticBunchCrossingTool.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} ${CORAL_LIBRARIES} AsgTools GaudiKernel TestTools AthenaKernel CxxUtils AthenaPoolUtilities xAODEventInfo xAODTrigger TrigConfL1Data )

atlas_add_test( ut_web_bunch_tool_test
                SOURCES
                test/ut_web_bunch_tool_test.cxx
                Root/BunchCrossing.cxx
                Root/BunchTrain.cxx
                Root/BunchCrossingToolBase.cxx
                Root/WebBunchCrossingTool.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} ${CORAL_LIBRARIES} AsgTools GaudiKernel TestTools AthenaKernel CxxUtils AthenaPoolUtilities xAODEventInfo xAODTrigger TrigConfL1Data )

# Install files from the package:
atlas_install_headers( TrigBunchCrossingTool )
atlas_install_python_modules( python/*.py )

