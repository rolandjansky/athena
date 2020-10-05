#Copyright (C) 2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( LArGeoCode )

# External dependencies:
find_package( GeoModelCore )

# Component(s) in the package:
atlas_add_library( LArGeoCode
                   src/*.cxx
                   PUBLIC_HEADERS LArGeoCode
                   PRIVATE_INCLUDE_DIRS ${GEOMODELCORE_INCLUDE_DIRS}
                   LINK_LIBRARIES StoreGateLib SGtests
                   PRIVATE_LINK_LIBRARIES ${GEOMODELCORE_LIBRARIES} GaudiKernel GeoModelInterfaces RDBAccessSvcLib )

