#Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( LArGeoTBEC )

# External dependencies:
find_package( GeoModel COMPONENTS GeoModelKernel )

# Component(s) in the package:
atlas_add_library( LArGeoTBECLib
                   src/*.cxx
                   PUBLIC_HEADERS LArGeoTBEC
                   PRIVATE_INCLUDE_DIRS ${GEOMODEL_INCLUDE_DIRS}
                   LINK_LIBRARIES ${GEOMODEL_LIBRARIES} GeoModelUtilities LArReadoutGeometry StoreGateLib
                   PRIVATE_LINK_LIBRARIES GaudiKernel LArG4RunControl LArGeoCode LArGeoEndcap LArGeoRAL RDBAccessSvcLib )

atlas_add_component( LArGeoTBEC
                     src/components/*.cxx
                     LINK_LIBRARIES LArGeoTBECLib )

