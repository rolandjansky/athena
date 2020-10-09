# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( VP1PRDSystems )

# External dependencies:
find_package( Coin3D )
find_package( Qt5 COMPONENTS Core Gui Widgets)
find_package( GeoModelCore )

# Generate UI files automatically:
set( CMAKE_AUTOUIC TRUE )
# Generate MOC files automatically:
set( CMAKE_AUTOMOC TRUE )

# Component(s) in the package:
atlas_add_library( VP1PRDSystems VP1PRDSystems/*.h src/*.cxx
   PUBLIC_HEADERS VP1PRDSystems
   PRIVATE_INCLUDE_DIRS ${COIN3D_INCLUDE_DIRS}
   ${CMAKE_CURRENT_BINARY_DIR}
   LINK_LIBRARIES ${GEOMODELCORE_LIBRARIES} GeoPrimitives InDetPrepRawData InDetReadoutGeometry MuonPrepRawData TrkPrepRawData TrkSpacePoint VP1Base VP1Utils Qt5::Core Qt5::Gui
   PRIVATE_LINK_LIBRARIES ${COIN3D_LIBRARIES} AthContainers EventPrimitives GeoModelUtilities InDetIdentifier MuonIdHelpersLib MuonReadoutGeometry TRT_ReadoutGeometry TrkCompetingRIOsOnTrack TrkMeasurementBase TrkRIO_OnTrack TrkSegment TrkSurfaces TrkTrack VP1GuideLineSystems VP1HEPVis )
