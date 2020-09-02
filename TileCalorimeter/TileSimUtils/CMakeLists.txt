# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name.
atlas_subdir( TileSimUtils )

# External dependencies.
find_package( ROOT COMPONENTS Core Tree )

# Component(s) in the package.
atlas_add_library( TileSimUtilsLib
   TileSimUtils/*.h src/*.cxx
   PUBLIC_HEADERS TileSimUtils
   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES CaloIdentifier CaloSimEvent AthenaBaseComps GaudiKernel
   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaKernel )

atlas_add_component( TileSimUtils
   src/components/*.cxx
   LINK_LIBRARIES TileSimUtilsLib )
