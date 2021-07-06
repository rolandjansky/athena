# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TileDetDescr )

# External dependencies:
find_package( GeoModel COMPONENTS GeoModelKernel )

# Component(s) in the package:
atlas_add_library( TileDetDescr
                   src/Tile*.cxx
                   PUBLIC_HEADERS TileDetDescr
                   INCLUDE_DIRS ${GEOMODEL_INCLUDE_DIRS}
                   LINK_LIBRARIES ${GEOMODEL_LIBRARIES} CaloIdentifier AthenaKernel Identifier CaloDetDescrLib RDBAccessSvcLib
                   PRIVATE_LINK_LIBRARIES AtlasDetDescr GaudiKernel TileIdentifier )

