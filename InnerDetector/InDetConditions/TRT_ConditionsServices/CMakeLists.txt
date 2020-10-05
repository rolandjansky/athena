################################################################################
# Package: TRT_ConditionsServices
################################################################################

# Declare the package name:
atlas_subdir( TRT_ConditionsServices )

# Possible extra dependencies:
set( extra_dep )
set( extra_lib )
if( NOT SIMULATIONBASE )
   set( extra_dep InnerDetector/InDetConditions/InDetByteStreamErrors )
   set( extra_lib InDetByteStreamErrors )
endif()

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaKernel
			  Control/StoreGate
                          DetectorDescription/GeoPrimitives
                          Event/EventPrimitives
                          GaudiKernel
                          InnerDetector/InDetConditions/InDetConditionsSummaryService
                          InnerDetector/InDetConditions/TRT_ConditionsData
                          PRIVATE
                          Control/AthenaBaseComps
                          Control/CxxUtils
                          Control/StoreGate
                          Database/AthenaPOOL/AthenaPoolUtilities
                          Database/RDBAccessSvc
                          Database/RegistrationServices
                          DetectorDescription/DetDescrCond/DetDescrConditions
                          DetectorDescription/GeoModel/GeoModelInterfaces
                          DetectorDescription/GeoModel/GeoModelUtilities
                          DetectorDescription/Identifier
                          Event/xAOD/xAODEventInfo
                          InnerDetector/InDetConditions/InDetCoolCoralClientUtils
                          InnerDetector/InDetDetDescr/InDetIdentifier
                          InnerDetector/InDetDetDescr/InDetReadoutGeometry
			  InnerDetector/InDetDetDescr/TRT_ReadoutGeometry
                          Tools/PathResolver
                          ${extra_dep} )

# External dependencies:
find_package( COOL COMPONENTS CoolKernel )
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )
find_package( ROOT COMPONENTS RIO Core Tree MathCore Hist pthread )

# Component(s) in the package:
atlas_add_library( TRT_ConditionsServicesLib
                   src/*.cxx
                   PUBLIC_HEADERS TRT_ConditionsServices
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS} ${COOL_INCLUDE_DIRS}
                   LINK_LIBRARIES AthenaKernel GeoPrimitives EventPrimitives GaudiKernel TRT_ConditionsData StoreGateLib SGtests InDetConditionsSummaryService
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} ${CORAL_LIBRARIES} ${COOL_LIBRARIES} AthenaBaseComps CxxUtils AthenaPoolUtilities DetDescrConditions GeoModelUtilities Identifier xAODEventInfo InDetCoolCoralClientUtils InDetIdentifier InDetReadoutGeometry TRT_ReadoutGeometry PathResolver RegistrationServicesLib ${extra_lib} RDBAccessSvcLib )

atlas_add_component( TRT_ConditionsServices
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS} ${COOL_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${CORAL_LIBRARIES} ${COOL_LIBRARIES} AthenaKernel GeoPrimitives EventPrimitives GaudiKernel TRT_ConditionsData AthenaBaseComps CxxUtils StoreGateLib SGtests AthenaPoolUtilities DetDescrConditions GeoModelUtilities Identifier xAODEventInfo EventInfo InDetCoolCoralClientUtils InDetIdentifier InDetReadoutGeometry TRT_ReadoutGeometry PathResolver TRT_ConditionsServicesLib InDetConditionsSummaryService ${extra_lib} RegistrationServicesLib )

atlas_install_python_modules( python/*.py )
