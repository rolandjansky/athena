# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( CavernInfraGeoModel )

# External dependencies:
find_package( GeoModelCore )

# Component(s) in the package:
atlas_add_library( CavernInfraGeoModelLib
                   CavernInfraGeoModel/*.h
                   INTERFACE
                   PUBLIC_HEADERS CavernInfraGeoModel
                   INCLUDE_DIRS ${GEOMODELCORE_INCLUDE_DIRS}
                   LINK_LIBRARIES ${GEOMODELCORE_LIBRARIES} AthenaKernel )

atlas_add_component( CavernInfraGeoModel
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES CavernInfraGeoModelLib GaudiKernel GeoModelInterfaces GeoModelUtilities RDBAccessSvcLib StoreGateLib )
