#Copyright (C) 2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( LArGeoTBBarrel )

# External dependencies:
find_package( GeoModelCore )

# Component(s) in the package:
atlas_add_library( LArGeoTBBarrel
                   src/*.cxx
                   PUBLIC_HEADERS LArGeoTBBarrel
                   PRIVATE_INCLUDE_DIRS ${GEOMODELCORE_INCLUDE_DIRS}
                   LINK_LIBRARIES ${GEOMODELCORE_LIBRARIES} StoreGateLib SGtests
                   PRIVATE_LINK_LIBRARIES GeoModelUtilities GaudiKernel LArGeoBarrel )

