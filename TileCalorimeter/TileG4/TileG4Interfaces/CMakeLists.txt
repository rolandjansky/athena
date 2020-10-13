# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TileG4Interfaces )

# External dependencies:
find_package( Geant4 )

atlas_add_library( TileG4InterfacesLib
                   TileG4Interfaces/*.h
                   INTERFACE
                   PUBLIC_HEADERS TileG4Interfaces
                   INCLUDE_DIRS ${GEANT4_INCLUDE_DIRS}
                   LINK_LIBRARIES ${GEANT4_LIBRARIES} GaudiKernel Identifier )
