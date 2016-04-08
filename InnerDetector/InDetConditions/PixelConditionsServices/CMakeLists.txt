################################################################################
# Package: PixelConditionsServices
################################################################################

# Declare the package name:
atlas_subdir( PixelConditionsServices )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Database/AthenaPOOL/AthenaPoolUtilities
                          GaudiKernel
                          InnerDetector/InDetConditions/InDetByteStreamErrors
                          InnerDetector/InDetConditions/InDetConditionsSummaryService
                          InnerDetector/InDetConditions/PixelConditionsData
                          PRIVATE
                          Control/SGTools
                          Control/StoreGate
                          Database/AthenaPOOL/RDBAccessSvc
                          Database/CoralDB
                          DetectorDescription/GeoModel/GeoModelInterfaces
                          DetectorDescription/GeoModel/GeoModelUtilities
                          DetectorDescription/Identifier
                          InnerDetector/InDetConditions/PixelConditionsTools
                          InnerDetector/InDetDetDescr/InDetIdentifier
                          InnerDetector/InDetDetDescr/InDetReadoutGeometry
                          InnerDetector/InDetDetDescr/PixelCabling
                          InnerDetector/InDetDetDescr/PixelGeoModel )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( CLHEP )
find_package( COOL COMPONENTS CoolKernel )
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )
find_package( ROOT COMPONENTS RIO Core Tree MathCore Hist pthread )

# Component(s) in the package:
atlas_add_component( PixelConditionsServices
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS} ${COOL_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${Boost_LIBRARIES} ${CORAL_LIBRARIES} ${COOL_LIBRARIES} ${CLHEP_LIBRARIES} AthenaBaseComps AthenaKernel AthenaPoolUtilities GaudiKernel PixelConditionsData SGTools StoreGateLib SGtests CoralDB GeoModelUtilities Identifier InDetIdentifier InDetReadoutGeometry )

# Install files from the package:
atlas_install_headers( PixelConditionsServices )
atlas_install_joboptions( share/*.py )

