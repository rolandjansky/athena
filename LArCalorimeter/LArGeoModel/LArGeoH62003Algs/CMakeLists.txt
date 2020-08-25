#Copyright (C) 2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( LArGeoH62003Algs )

# External dependencies:
find_package( GeoModelCore )

# Component(s) in the package:
atlas_add_component( LArGeoH62003Algs
                     src/*.cxx
                     src/components/*.cxx
                     PRIVATE_INCLUDE_DIRS ${GEOMODELCORE_INCLUDE_DIRS}
                     PRIVATE_LINK_LIBRARIES ${GEOMODELCORE_LIBRARIES} GeoModelUtilities LArReadoutGeometry StoreGateLib SGtests GaudiKernel LArG4RunControl LArGeoFcal LArGeoH6Cryostats RDBAccessSvcLib )
