#Copyright (C) 2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( LArGeoH62002Algs )

# External dependencies:
find_package( GeoModelCore )

# Component(s) in the package:
atlas_add_component( LArGeoH62002Algs
                     src/*.cxx
                     src/components/*.cxx
                     PRIVATE_INCLUDE_DIRS ${GEOMODELCORE_INCLUDE_DIRS}
                     PRIVATE_LINK_LIBRARIES ${GEOMODELCORE_LIBRARIES} GeoModelUtilities LArReadoutGeometry StoreGateLib SGtests GaudiKernel LArG4RunControl LArGeoCode LArGeoEndcap LArGeoH6Cryostats RDBAccessSvcLib )
