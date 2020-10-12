# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( VP1GeometryPlugin )

# External dependencies:
find_package( Qt5 COMPONENTS Core Gui Widgets )

# Generate MOC files automatically:
set( CMAKE_AUTOMOC TRUE )

atlas_add_library( VP1GeometryPlugin
   VP1GeometryPlugin/*.h src/*.cxx src/*.qrc 
   PUBLIC_HEADERS VP1GeometryPlugin
   LINK_LIBRARIES Qt5::Core Qt5::Gui Qt5::Widgets
   PRIVATE_LINK_LIBRARIES VP1Base VP1GuideLineSystems VP1GeometrySystems )
