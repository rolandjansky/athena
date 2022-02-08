#Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( LArGeoEndcap )

# External dependencies:
find_package( GeoModel COMPONENTS GeoModelKernel )

# Component(s) in the package:
atlas_add_library( LArGeoEndcap
                   src/*.cxx
                   PUBLIC_HEADERS LArGeoEndcap
                   INCLUDE_DIRS ${GEOMODEL_INCLUDE_DIRS}
                   LINK_LIBRARIES ${GEOMODEL_LIBRARIES} LArGeoFcal LArGeoHec StoreGateLib CaloDetDescrLib
                   PRIVATE_LINK_LIBRARIES CaloIdentifier GeoModelUtilities GaudiKernel LArGeoCode LArGeoMiniFcal RDBAccessSvcLib )

