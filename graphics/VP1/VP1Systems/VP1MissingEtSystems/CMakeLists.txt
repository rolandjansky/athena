# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( VP1MissingEtSystems )

# External dependencies:
find_package( CLHEP )
find_package( Coin3D )
find_package( Qt5 COMPONENTS Core Gui Widgets )

# Generate UI files automatically:
set( CMAKE_AUTOUIC TRUE )
# Generate MOC files automatically:
set( CMAKE_AUTOMOC TRUE )
# Generate resource files automatically:
set( CMAKE_AUTORCC TRUE )
# to let CMake find .h files automatically-generated from .ui files
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Component(s) in the package:
atlas_add_library( VP1MissingEtSystems VP1MissingEtSystems/*.h src/*.h src/*.cxx src/*.qrc 
                   PUBLIC_HEADERS VP1MissingEtSystems
                   INCLUDE_DIRS ${COIN3D_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                   PRIVATE_DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${COIN3D_LIBRARIES} VP1Base GL
                   PRIVATE_LINK_LIBRARIES ${CLHEP_LIBRARIES} MissingETEvent VP1Utils )

