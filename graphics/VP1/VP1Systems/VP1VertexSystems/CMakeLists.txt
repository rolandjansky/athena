# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( VP1VertexSystems )

# External dependencies:
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
atlas_add_library( VP1VertexSystems VP1VertexSystems/*.h src/*.h src/*.cxx src/*.qrc  
                   PUBLIC_HEADERS VP1VertexSystems
                   PRIVATE_INCLUDE_DIRS ${COIN3D_INCLUDE_DIRS}
                   LINK_LIBRARIES VP1Base GL Qt5::Core Qt5::Gui
                   PRIVATE_LINK_LIBRARIES ${COIN3D_LIBRARIES} AtlasHepMCLib GaudiKernel GeneratorObjects GeoPrimitives TrkParticleBase TrkTrack TrkTrackLink VP1Utils VxVertex )
