###############################################################################
# Package: SCT_ConditionsServices
################################################################################

# Declare the package name:
atlas_subdir( SCT_ConditionsServices )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaKernel
                          Control/StoreGate
                          DetectorDescription/Identifier
                          GaudiKernel
                          InnerDetector/InDetConditions/InDetByteStreamErrors
                          InnerDetector/InDetConditions/InDetConditionsSummaryService
                          InnerDetector/InDetConditions/SCT_ConditionsData
                          PRIVATE
                          Control/AthenaBaseComps
                          Control/CxxUtils
                          Database/AthenaPOOL/AthenaPoolUtilities
                          Database/AthenaPOOL/RDBAccessSvc
                          Database/RegistrationServices
                          DetectorDescription/GeoModel/GeoModelInterfaces
                          DetectorDescription/GeoModel/GeoModelUtilities
                          Event/EventInfo
                          Event/xAOD/xAODEventInfo
                          InnerDetector/InDetConditions/SiPropertiesSvc
                          InnerDetector/InDetDetDescr/InDetIdentifier
                          InnerDetector/InDetDetDescr/InDetReadoutGeometry
                          InnerDetector/InDetDetDescr/SCT_Cabling
                          Tools/PathResolver 
			  AtlasTest/TestTools )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( CLHEP )
find_package( COOL COMPONENTS CoolKernel )
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )
find_package( GTest )
find_package( GMock )

# Component(s) in the package:
atlas_add_component( SCT_ConditionsServices
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS} ${COOL_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${Boost_LIBRARIES} ${CORAL_LIBRARIES} ${COOL_LIBRARIES} ${CLHEP_LIBRARIES} AthenaKernel Identifier GaudiKernel SCT_ConditionsData AthenaBaseComps StoreGateLib SGtests AthenaPoolUtilities GeoModelUtilities EventInfo xAODEventInfo SiPropertiesSvcLib InDetIdentifier InDetReadoutGeometry SCT_CablingLib PathResolver SCT_ConditionsServicesLib )

atlas_add_library ( SCT_ConditionsServicesLib
		    src/*.cxx
		    PUBLIC_HEADERS SCT_ConditionsServices
		    INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS} ${COOL_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
		    LINK_LIBRARIES ${ROOT_LIBRARIES} ${Boost_LIBRARIES} ${CORAL_LIBRARIES} ${COOL_LIBRARIES} ${CLHEP_LIBRARIES} AthenaKernel Identifier GaudiKernel SCT_ConditionsData AthenaBaseComps StoreGateLib SGtests AthenaPoolUtilities GeoModelUtilities EventInfo xAODEventInfo SiPropertiesSvcLib InDetIdentifier InDetReadoutGeometry SCT_CablingLib PathResolver)

# Add unit tests
atlas_add_test( SCT_RODVetoSvc_test
 		SOURCES test/SCT_RODVetoSvc_test.cxx
 		INCLUDE_DIRS ${GTEST_INCLUDE_DIRS}  ${GMOCK_INCLUDE_DIRS}
 		LINK_LIBRARIES TestTools SCT_ConditionsServicesLib ${GTEST_LIBRARIES} ${GMOCK_LIBRARIES}
 		ENVIRONMENT "JOBOPTSEARCHPATH=${CMAKE_CURRENT_SOURCE_DIR}/share" ) 

# Install files from the package:
atlas_install_headers( SCT_ConditionsServices )

