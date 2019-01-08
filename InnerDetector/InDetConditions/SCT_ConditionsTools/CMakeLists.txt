###############################################################################
# Package: SCT_ConditionsTools
################################################################################

# Declare the package name:
atlas_subdir( SCT_ConditionsTools )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaKernel
                          DetectorDescription/Identifier
                          GaudiKernel
                          InnerDetector/InDetConditions/InDetConditionsSummaryService
                          InnerDetector/InDetConditions/SCT_ConditionsData
                          PRIVATE
                          Control/AthenaBaseComps
                          Control/StoreGate
                          Event/EventInfo
                          Event/xAOD/xAODEventInfo
                          Database/RDBAccessSvc
                          DetectorDescription/GeoModel/GeoModelInterfaces
                          DetectorDescription/GeoModel/GeoModelUtilities
                          InnerDetector/InDetConditions/InDetByteStreamErrors
                          InnerDetector/InDetConditions/SiPropertiesSvc
                          InnerDetector/InDetDetDescr/InDetIdentifier
                          InnerDetector/InDetDetDescr/InDetReadoutGeometry
                          InnerDetector/InDetDetDescr/SCT_Cabling
                          AtlasTest/TestTools )

# External dependencies:
find_package( CLHEP )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )
find_package( GTest )
find_package( GMock )

# Component(s) in the package:
atlas_add_component ( SCT_ConditionsTools
                      src/components/*.cxx
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} AthenaKernel Identifier GeoModelUtilities GaudiKernel AthenaBaseComps StoreGateLib SGtests EventInfo xAODEventInfo SCT_ConditionsData InDetByteStreamErrors InDetIdentifier InDetReadoutGeometry SCT_CablingLib SiPropertiesSvcLib SCT_ConditionsToolsLib )


atlas_add_library( SCT_ConditionsToolsLib
                   src/*.cxx
                   PUBLIC_HEADERS SCT_ConditionsTools
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                   LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} AthenaKernel Identifier GeoModelUtilities GaudiKernel AthenaBaseComps StoreGateLib SGtests EventInfo xAODEventInfo SCT_ConditionsData SiPropertiesSvcLib InDetByteStreamErrors InDetIdentifier InDetReadoutGeometry SCT_CablingLib )

# Add unit tests
atlas_add_test( SCT_RODVetoTool_test
                SOURCES test/SCT_RODVetoTool_test.cxx
                INCLUDE_DIRS ${GTEST_INCLUDE_DIRS}  ${GMOCK_INCLUDE_DIRS}
                LINK_LIBRARIES TestTools SCT_ConditionsToolsLib ${GTEST_LIBRARIES} ${GMOCK_LIBRARIES}
                ENVIRONMENT "JOBOPTSEARCHPATH=${CMAKE_CURRENT_SOURCE_DIR}/share" )

# Install files from the package:
atlas_install_headers( SCT_ConditionsTools )
atlas_install_python_modules( python/*.py )
atlas_install_scripts( share/*.py )
