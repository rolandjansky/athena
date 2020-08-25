################################################################################
# Package: GeoSpecialShapes
################################################################################

# Declare the package name:
atlas_subdir( GeoSpecialShapes )

# External dependencies:
find_package( CLHEP )
find_package( GSL )
find_package( ROOT COMPONENTS Matrix Core Tree MathCore Hist RIO pthread MathMore Minuit Minuit2 Physics HistPainter Rint )
find_package( GeoModelCore )

if(NOT BUILDVP1LIGHT)
# Component(s) in the package:
atlas_add_library( GeoSpecialShapes
                   src/*.cxx
                   src/LArWheelCalculator_Impl/*.cxx
                   PUBLIC_HEADERS GeoSpecialShapes
                   INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${CLHEP_LIBRARIES} ${GEOMODELCORE_LIBRARIES} AthenaKernel StoreGateLib SGtests
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} ${GSL_LIBRARIES} AthenaKernel CxxUtils GeoModelUtilities GaudiKernel RDBAccessSvcLib )

atlas_add_dictionary( LArGeoCheckerDict
                      GeoSpecialShapes/LArGeoCheckerDict.h
                      GeoSpecialShapes/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} ${GSL_LIBRARIES} ${CLHEP_LIBRARIES} ${GEOMODELCORE_LIBRARIES} StoreGateLib SGtests AthenaKernel CxxUtils GeoModelUtilities GaudiKernel GeoSpecialShapes )

atlas_add_dictionary( LArWheelEnums
                      GeoSpecialShapes/LArWheelCalculatorEnums.h
                      GeoSpecialShapes/selectionEnums.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} ${GSL_LIBRARIES} ${CLHEP_LIBRARIES} ${GEOMODELCORE_LIBRARIES} StoreGateLib SGtests AthenaKernel CxxUtils GeoModelUtilities GaudiKernel GeoSpecialShapes )
endif()

if(BUILDVP1LIGHT)
# Component(s) in the package:
atlas_add_library( GeoSpecialShapes
                   src/*.cxx
                   src/LArWheelCalculator_Impl/*.cxx
                   PUBLIC_HEADERS GeoSpecialShapes
                   INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${CLHEP_LIBRARIES} GeoModelKernel
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} ${GSL_LIBRARIES} CxxUtils GeoModelUtilities )

atlas_add_dictionary( LArGeoCheckerDict
                      GeoSpecialShapes/LArGeoCheckerDict.h
                      GeoSpecialShapes/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} ${GSL_LIBRARIES} ${CLHEP_LIBRARIES} ${GEOMODELCORE_LIBRARIES} CxxUtils GeoModelUtilities GeoSpecialShapes )

atlas_add_dictionary( LArWheelEnums
                      GeoSpecialShapes/LArWheelCalculatorEnums.h
                      GeoSpecialShapes/selectionEnums.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} ${GSL_LIBRARIES} ${CLHEP_LIBRARIES} GeoModelKernel CxxUtils GeoModelUtilities GeoSpecialShapes )
endif()
