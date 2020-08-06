#Copyright (C) 2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( LArGeoBarrel )

# External dependencies:
find_package( GeoModelCore )

# Component(s) in the package:
atlas_add_library( LArGeoBarrel
                   src/*.cxx
                   PUBLIC_HEADERS LArGeoBarrel
                   INCLUDE_DIRS ${GEOMODELCORE_INCLUDE_DIRS}
                   LINK_LIBRARIES ${GEOMODELCORE_LIBRARIES} LArGeoCode StoreGateLib SGtests
                   PRIVATE_LINK_LIBRARIES GeoModelUtilities GaudiKernel LArReadoutGeometry RDBAccessSvcLib )

