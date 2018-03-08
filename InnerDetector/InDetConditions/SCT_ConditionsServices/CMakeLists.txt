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
                          InnerDetector/InDetConditions/SCT_ConditionsTools
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
                          Tools/PathResolver )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( CLHEP )
find_package( COOL COMPONENTS CoolKernel )
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( SCT_ConditionsServices
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS} ${COOL_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${Boost_LIBRARIES} ${CORAL_LIBRARIES} ${COOL_LIBRARIES} ${CLHEP_LIBRARIES} AthenaKernel Identifier GaudiKernel SCT_ConditionsData SCT_ConditionsToolsLib AthenaBaseComps StoreGateLib SGtests AthenaPoolUtilities GeoModelUtilities EventInfo xAODEventInfo SiPropertiesSvcLib InDetIdentifier InDetReadoutGeometry SCT_CablingLib PathResolver SCT_ConditionsServicesLib )

atlas_add_library ( SCT_ConditionsServicesLib
		    src/*.cxx
		    PUBLIC_HEADERS SCT_ConditionsServices
		    INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS} ${COOL_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
		    LINK_LIBRARIES ${ROOT_LIBRARIES} ${Boost_LIBRARIES} ${CORAL_LIBRARIES} ${COOL_LIBRARIES} ${CLHEP_LIBRARIES} AthenaKernel Identifier GaudiKernel SCT_ConditionsData SCT_ConditionsToolsLib AthenaBaseComps StoreGateLib SGtests AthenaPoolUtilities GeoModelUtilities EventInfo xAODEventInfo SiPropertiesSvcLib InDetIdentifier InDetReadoutGeometry SCT_CablingLib PathResolver)

# Install files from the package:
atlas_install_headers( SCT_ConditionsServices )
atlas_install_python_modules( python/*.py )
