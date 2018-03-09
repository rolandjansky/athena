################################################################################
# Package: GeoSpecialShapes
################################################################################

# Declare the package name:
atlas_subdir( GeoSpecialShapes )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/SGTools
                          Control/StoreGate
                          DetectorDescription/GeoModel/GeoModelKernel
                          PRIVATE
                          Control/AthenaKernel
                          Control/CxxUtils
                          Database/AthenaPOOL/RDBAccessSvc
                          DetectorDescription/GeoModel/GeoModelInterfaces
                          DetectorDescription/GeoModel/GeoModelUtilities
                          GaudiKernel )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( CLHEP )
find_package( GSL )
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )
find_package( ROOT COMPONENTS Matrix Core Tree MathCore Hist RIO pthread MathMore Minuit Minuit2 Physics HistPainter Rint )

# Component(s) in the package:
atlas_add_library( GeoSpecialShapes
                   src/*.cxx
                   src/LArWheelCalculator_Impl/*.cxx
                   PUBLIC_HEADERS GeoSpecialShapes
                   INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                   DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${CLHEP_LIBRARIES} SGTools GeoModelKernel StoreGateLib SGtests
                   PRIVATE_LINK_LIBRARIES ${Boost_LIBRARIES} ${CORAL_LIBRARIES} ${ROOT_LIBRARIES} ${GSL_LIBRARIES} AthenaKernel CxxUtils GeoModelUtilities GaudiKernel )

atlas_add_dictionary( LArGeoCheckerDict
                      GeoSpecialShapes/LArGeoCheckerDict.h
                      GeoSpecialShapes/selection.xml
                      INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                      LINK_LIBRARIES ${Boost_LIBRARIES} ${CORAL_LIBRARIES} ${ROOT_LIBRARIES} ${GSL_LIBRARIES} ${CLHEP_LIBRARIES} SGTools StoreGateLib SGtests GeoModelKernel AthenaKernel CxxUtils GeoModelUtilities GaudiKernel GeoSpecialShapes )

atlas_add_dictionary( LArWheelEnums
                      GeoSpecialShapes/LArWheelCalculatorEnums.h
                      GeoSpecialShapes/selectionEnums.xml
                      INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                      LINK_LIBRARIES ${Boost_LIBRARIES} ${CORAL_LIBRARIES} ${ROOT_LIBRARIES} ${GSL_LIBRARIES} ${CLHEP_LIBRARIES} SGTools StoreGateLib SGtests GeoModelKernel AthenaKernel CxxUtils GeoModelUtilities GaudiKernel GeoSpecialShapes )

