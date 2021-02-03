# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( CavernInfraGeoModel )

# External dependencies:
find_package( GeoModel COMPONENTS GeoModelKernel )

# Component(s) in the package:
atlas_add_component( CavernInfraGeoModel
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${GEOMODEL_INCLUDE_DIRS}	
                     LINK_LIBRARIES ${GEOMODEL_LIBRARIES} AthenaKernel GaudiKernel GeoModelInterfaces GeoModelUtilities RDBAccessSvcLib StoreGateLib )
