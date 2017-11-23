################################################################################
# Package: TRT_ConditionsServices
################################################################################

# Declare the package name:
atlas_subdir( TRT_ConditionsServices )

# Possible extra dependencies:
set( extra_dep )
if( NOT SIMULATIONBASE )
   set( extra_dep InnerDetector/InDetConditions/InDetByteStreamErrors )
endif()

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaKernel
                          DetectorDescription/GeoPrimitives
                          Event/EventPrimitives
                          GaudiKernel
                          InnerDetector/InDetConditions/InDetConditionsSummaryService
                          InnerDetector/InDetConditions/TRT_ConditionsData
                          PRIVATE
                          Control/AthenaBaseComps
                          Control/StoreGate
                          Database/AthenaPOOL/AthenaPoolUtilities
                          Database/AthenaPOOL/RDBAccessSvc
                          Database/RegistrationServices
                          DetectorDescription/DetDescrCond/DetDescrConditions
                          DetectorDescription/GeoModel/GeoModelInterfaces
                          DetectorDescription/GeoModel/GeoModelUtilities
                          DetectorDescription/Identifier
                          Event/EventInfo
                          InnerDetector/InDetConditions/InDetCoolCoralClientUtils
                          InnerDetector/InDetDetDescr/InDetIdentifier
                          InnerDetector/InDetDetDescr/InDetReadoutGeometry
                          InnerDetector/InDetRecTools/InDetRecToolInterfaces
                          Tools/PathResolver
                          ${extra_dep} )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( COOL COMPONENTS CoolKernel )
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )
find_package( ROOT COMPONENTS RIO Core Tree MathCore Hist pthread )

# Component(s) in the package:
atlas_add_library( TRT_ConditionsServicesLib
                   src/*.cxx
                   PUBLIC_HEADERS TRT_ConditionsServices
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS} ${COOL_INCLUDE_DIRS}
                   LINK_LIBRARIES AthenaKernel GeoPrimitives EventPrimitives GaudiKernel TRT_ConditionsData StoreGateLib SGtests
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} ${Boost_LIBRARIES} ${CORAL_LIBRARIES} ${COOL_LIBRARIES} AthenaBaseComps AthenaPoolUtilities DetDescrConditions GeoModelUtilities Identifier EventInfo InDetCoolCoralClientUtils InDetIdentifier InDetReadoutGeometry PathResolver )

atlas_add_component( TRT_ConditionsServices
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS} ${COOL_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${Boost_LIBRARIES} ${CORAL_LIBRARIES} ${COOL_LIBRARIES} AthenaKernel GeoPrimitives EventPrimitives GaudiKernel TRT_ConditionsData AthenaBaseComps StoreGateLib SGtests AthenaPoolUtilities DetDescrConditions GeoModelUtilities Identifier EventInfo InDetCoolCoralClientUtils InDetIdentifier InDetReadoutGeometry PathResolver TRT_ConditionsServicesLib )

atlas_install_python_modules( python/*.py )
