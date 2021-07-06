# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( GeoSpecialShapes )

# External dependencies:
find_package( CLHEP )
find_package( GSL )
find_package( ROOT COMPONENTS Core MathCore Matrix )
find_package( GeoModel COMPONENTS GeoModelKernel )

# The list of linked libraries depends on what project is being built.
set( public_libs )
set( private_libs )
if( NOT BUILDVP1LIGHT )
   set( extra_public_libs AthenaKernel StoreGateLib )
   set( extra_private_libs AthenaKernel GaudiKernel RDBAccessSvcLib )
endif()

# Component(s) in the package:
atlas_add_library( GeoSpecialShapes
   GeoSpecialShapes/*.h src/*.cxx src/LArWheelCalculator_Impl/*.cxx
   PUBLIC_HEADERS GeoSpecialShapes
   INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS} ${GEOMODEL_INCLUDE_DIRS}
   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${GSL_INCLUDE_DIRS}
   DEFINITIONS ${CLHEP_DEFINITIONS}
   LINK_LIBRARIES ${CLHEP_LIBRARIES} ${GEOMODEL_LIBRARIES} ${extra_public_libs}
   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} ${GSL_LIBRARIES} CxxUtils
   GeoModelUtilities ${extra_private_libs} )

atlas_add_dictionary( LArGeoCheckerDict
   GeoSpecialShapes/LArGeoCheckerDict.h
   GeoSpecialShapes/selection.xml
   LINK_LIBRARIES GeoSpecialShapes )

atlas_add_dictionary( LArWheelEnums
   GeoSpecialShapes/LArWheelCalculatorEnums.h
   GeoSpecialShapes/selectionEnums.xml )
