#Copyright (C) 2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( LArGeoEndcap )

# External dependencies:
find_package( GeoModelCore )

# Component(s) in the package:
atlas_add_library( LArGeoEndcap
                   src/*.cxx
                   PUBLIC_HEADERS LArGeoEndcap
                   INCLUDE_DIRS ${GEOMODELCORE_INCLUDE_DIRS}
                   LINK_LIBRARIES ${GEOMODELCORE_LIBRARIES} LArGeoFcal LArGeoHec StoreGateLib SGtests CaloDetDescrLib
                   PRIVATE_LINK_LIBRARIES CaloIdentifier GeoModelUtilities GeoSpecialShapes GaudiKernel LArGeoCode LArGeoMiniFcal RDBAccessSvcLib )

