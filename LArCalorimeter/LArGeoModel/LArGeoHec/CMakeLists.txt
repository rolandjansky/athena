#Copyright (C) 2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( LArGeoHec )

# External dependencies:
find_package( GeoModelCore )

# Component(s) in the package:
atlas_add_library( LArGeoHec
                   src/*.cxx
                   PUBLIC_HEADERS LArGeoHec
                   PRIVATE_INCLUDE_DIRS ${GEOMODELCORE_INCLUDE_DIRS}
                   LINK_LIBRARIES StoreGateLib SGtests
                   PRIVATE_LINK_LIBRARIES ${GEOMODELCORE_LIBRARIES} AthenaKernel GeoModelUtilities GaudiKernel RDBAccessSvcLib )

