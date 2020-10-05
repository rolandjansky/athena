# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( VP1BanksSystems )

# External package(s):
find_package( Qt5 COMPONENTS Core Gui Widgets )

# Generate UI files automatically:
set( CMAKE_AUTOUIC TRUE )
# Generate MOC files automatically:
set( CMAKE_AUTOMOC TRUE )

# Component(s) in the package:
atlas_add_library( VP1BanksSystems
   VP1BanksSystems/*.h src/*.cxx
   PUBLIC_HEADERS VP1BanksSystems
   PRIVATE_INCLUDE_DIRS ${CMAKE_CURRENT_BINARY_DIR}
   LINK_LIBRARIES VP1Base
   PRIVATE_LINK_LIBRARIES AthenaKernel GaudiKernel VP1Utils )
