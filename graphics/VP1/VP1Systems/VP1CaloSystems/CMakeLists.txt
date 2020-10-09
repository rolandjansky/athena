# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( VP1CaloSystems )

# External dependencies:
find_package( Coin3D )
find_package( Qt5 COMPONENTS Core Gui Widgets )
#find_package( Qwt )
find_package( GeoModelCore )

# Generate UI files automatically:
set( CMAKE_AUTOUIC TRUE )
# Generate MOC files automatically:
set( CMAKE_AUTOMOC TRUE )

# Component(s) in the package:
atlas_add_library( VP1CaloSystems
   VP1CaloSystems/*.h src/*.cxx
   PUBLIC_HEADERS VP1CaloSystems
   PRIVATE_INCLUDE_DIRS ${COIN3D_INCLUDE_DIRS} ${QWT_INCLUDE_DIRS}
   ${CMAKE_CURRENT_BINARY_DIR}
   LINK_LIBRARIES ${GEOMODELCORE_LIBRARIES} GaudiKernel GeoPrimitives Identifier VP1Base Qt5::Core
   PRIVATE_LINK_LIBRARIES ${COIN3D_LIBRARIES} ${QWT_LIBRARIES} CaloConditions CaloDetDescrLib CaloEvent CaloIdentifier GeoModelUtilities LArCablingLib LArIdentifier LArRawEvent StoreGateLib TileConditionsLib TileDetDescr TileEvent TileIdentifier VP1HEPVis VP1Utils Qt5::Gui )
