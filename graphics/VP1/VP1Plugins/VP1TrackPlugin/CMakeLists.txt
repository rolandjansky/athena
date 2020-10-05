# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( VP1TrackPlugin )

# External dependencies:
find_package( Qt5 COMPONENTS Core Gui Widgets )

# Generate MOC files automatically:
set( CMAKE_AUTOMOC TRUE )

atlas_add_library( VP1TrackPlugin VP1TrackPlugin/*.h src/*.cxx src/*.qrc 
   PUBLIC_HEADERS VP1TrackPlugin
   INCLUDE_DIRS ${QT5_INCLUDE_DIRS} 
   PRIVATE_INCLUDE_DIRS tmpqt_extraheaders/ 
   LINK_LIBRARIES Qt5::Core Qt5::Gui Qt5::Widgets
   PRIVATE_LINK_LIBRARIES VP1Base VP1PlugUtils VP1GeometrySystems VP1GuideLineSystems VP1PRDSystems VP1RawDataSystems VP1SimHitSystems VP1TrackSystems VP1VertexSystems
)
