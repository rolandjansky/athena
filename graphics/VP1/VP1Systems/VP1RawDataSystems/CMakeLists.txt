# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( VP1RawDataSystems )

# External dependencies:
find_package( Coin3D )
find_package( Eigen )
find_package( Qt5 COMPONENTS Core Gui Widgets )
find_package( GeoModelCore )

# Generate UI files automatically:
set( CMAKE_AUTOUIC TRUE )
# Generate MOC files automatically:
set( CMAKE_AUTOMOC TRUE )
# Generate resource files automatically:
set( CMAKE_AUTORCC TRUE )
# to let CMake find .h files automatically-generated from .ui files
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Component(s) in the package:
atlas_add_library( VP1RawDataSystems VP1RawDataSystems/*.h src/*.h src/*.cxx src/*.qrc  
                   PUBLIC_HEADERS VP1RawDataSystems
                   INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${COIN3D_INCLUDE_DIRS}
                   FILES LUCID_RawEvent/LUCID_Digit.h
                   LINK_LIBRARIES ${EIGEN_LIBRARIES} GeoPrimitives VP1Base GL
                   PRIVATE_LINK_LIBRARIES ${COIN3D_LIBRARIES} ${GEOMODELCORE_LIBRARIES} EventPrimitives LUCID_RawEvent GaudiKernel InDetIdentifier InDetReadoutGeometry PixelReadoutGeometry SCT_ReadoutGeometry TRT_ReadoutGeometry InDetBCM_RawData InDetRawData VP1HEPVis VP1Utils )

