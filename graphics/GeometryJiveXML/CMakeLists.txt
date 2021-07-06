# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( GeometryJiveXML )

# External dependencies:
find_package( GeoModel COMPONENTS GeoModelKernel )
find_package( CLHEP )

# Component(s) in the package:
atlas_add_component( GeometryJiveXML
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS} ${GEOMODEL_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} ${GEOMODEL_LIBRARIES} AthenaBaseComps CaloDetDescrLib GaudiKernel InDetIdentifier InDetReadoutGeometry JiveXMLLib LArReadoutGeometry MuonIdHelpersLib MuonReadoutGeometry PixelReadoutGeometry SCT_ReadoutGeometry StoreGateLib TRT_ReadoutGeometry TileDetDescr )
