#Copyright (C) 2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( LArGeoTBEC )

# External dependencies:
find_package( GeoModelCore )

# Component(s) in the package:
atlas_add_library( LArGeoTBECLib
                   src/*.cxx
                   PUBLIC_HEADERS LArGeoTBEC
                   PRIVATE_INCLUDE_DIRS ${GEOMODELCORE_INCLUDE_DIRS}
                   LINK_LIBRARIES ${GEOMODELCORE_LIBRARIES} GeoModelUtilities LArReadoutGeometry StoreGateLib SGtests
                   PRIVATE_LINK_LIBRARIES GeoSpecialShapes GaudiKernel LArG4RunControl LArGeoCode LArGeoEndcap LArGeoRAL RDBAccessSvcLib )

atlas_add_component( LArGeoTBEC
                     src/components/*.cxx
                     LINK_LIBRARIES LArGeoTBECLib )

