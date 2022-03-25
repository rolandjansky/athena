# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( VP1MCPlugin )

# External dependencies:
find_package( Qt5 COMPONENTS Core Gui Widgets )

# Generate UI files automatically:
set( CMAKE_AUTOUIC TRUE )
# Generate MOC files automatically:
set( CMAKE_AUTOMOC TRUE )
# Generate resource files automatically:
set( CMAKE_AUTORCC TRUE )

atlas_add_library( VP1MCPlugin VP1MCPlugin/*.h src/*.cxx
   PUBLIC_HEADERS VP1MCPlugin
   INCLUDE_DIRS ${QT5_INCLUDE_DIRS}
   PRIVATE_INCLUDE_DIRS tmpqt_extraheaders/
   LINK_LIBRARIES Qt5::Core Qt5::Gui Qt5::Widgets
   PRIVATE_LINK_LIBRARIES VP1Base VP1PlugUtils VP1GeometrySystems VP1MCSystems )


