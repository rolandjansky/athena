################################################################################
# Package: SCT_ConditionsServices
################################################################################

# Declare the package name:
atlas_subdir( SCT_ConditionsServices )

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
                          Database/AthenaPOOL/AthenaPoolUtilities
                          Database/AthenaPOOL/RDBAccessSvc
                          Database/RegistrationServices
                          DetectorDescription/GeoModel/GeoModelInterfaces
                          DetectorDescription/GeoModel/GeoModelUtilities
                          Event/EventInfo
                          InnerDetector/InDetConditions/InDetByteStreamErrors
                          InnerDetector/InDetConditions/SiPropertiesSvc
                          InnerDetector/InDetDetDescr/InDetIdentifier
                          InnerDetector/InDetDetDescr/InDetReadoutGeometry
                          InnerDetector/InDetDetDescr/PixelGeoModel
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
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS} ${COOL_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${Boost_LIBRARIES} ${CORAL_LIBRARIES} ${COOL_LIBRARIES} ${CLHEP_LIBRARIES} AthenaKernel Identifier GaudiKernel SCT_ConditionsData AthenaBaseComps StoreGateLib SGtests AthenaPoolUtilities GeoModelUtilities EventInfo SiPropertiesSvcLib InDetIdentifier InDetReadoutGeometry SCT_CablingLib PathResolver )

# Install files from the package:
atlas_install_headers( SCT_ConditionsServices )

