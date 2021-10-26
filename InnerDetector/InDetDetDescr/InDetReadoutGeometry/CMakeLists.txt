# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( InDetReadoutGeometry )

# External dependencies:
find_package( CLHEP )
find_package( GeoModel COMPONENTS GeoModelKernel )

# Component(s) in the package:
atlas_add_library( InDetReadoutGeometry
                   src/*.cxx
                   PUBLIC_HEADERS InDetReadoutGeometry
                   INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                   DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${CLHEP_LIBRARIES} ${GEOMODEL_LIBRARIES} AthenaKernel AtlasDetDescr CxxUtils GeoPrimitives Identifier GaudiKernel InDetIdentifier TrkDetElementBase ReadoutGeometryBase TrkSurfaces TrkEventPrimitives StoreGateLib DetDescrConditions InDetCondTools InDetGeoModelUtils AthenaBaseComps
                   PRIVATE_LINK_LIBRARIES AthenaPoolUtilities IdDictDetDescr GeoModelUtilities )
