#Copyright (C) 2021 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( LArGeoFcal )

# External dependencies:
find_package( GeoModel COMPONENTS GeoModelKernel )

# Component(s) in the package:
atlas_add_library( LArGeoFcal
                   src/*.cxx
                   PUBLIC_HEADERS LArGeoFcal
                   INCLUDE_DIRS ${GEOMODEL_INCLUDE_DIRS}
                   LINK_LIBRARIES ${GEOMODEL_LIBRARIES} StoreGateLib
                   PRIVATE_LINK_LIBRARIES GeoModelUtilities GaudiKernel LArReadoutGeometry RDBAccessSvcLib  )

