# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( VP1BPhysSystems )

# External dependencies:
find_package( Coin3D )
find_package( ROOT COMPONENTS Core Tree RIO )
find_package( Qt5 COMPONENTS Core Widgets)

# Generate UI files automatically:
set( CMAKE_AUTOUIC TRUE )
# Generate MOC files automatically:
set( CMAKE_AUTOMOC TRUE )
# Generate resource files automatically:
set( CMAKE_AUTORCC TRUE )
# to let CMake find .h files automatically-generated from .ui files
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Component(s) in the package:
atlas_add_library( VP1BPhysSystems VP1BPhysSystems/*.h src/*.h src/*.cxx src/*.qrc  
                   PUBLIC_HEADERS VP1BPhysSystems
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${COIN3D_INCLUDE_DIRS}
                   LINK_LIBRARIES GeoPrimitives VP1Base VP1TrackSystems GL
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} ${COIN3D_LIBRARIES} EventInfo Particle StoreGateLib TrkExInterfaces TrkParameters TrkTrack VP1Utils )
