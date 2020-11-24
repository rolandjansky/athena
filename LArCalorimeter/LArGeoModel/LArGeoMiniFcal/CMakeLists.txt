# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( LArGeoMiniFcal )

# External dependencies:
find_package( Eigen )
find_package( GeoModelCore )

# Component(s) in the package:
atlas_add_library( LArGeoMiniFcal
                   src/*.cxx
                   PUBLIC_HEADERS LArGeoMiniFcal
                   INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS} ${GEOMODELCORE_INCLUDE_DIRS}
                   LINK_LIBRARIES ${EIGEN_LIBRARIES} ${GEOMODELCORE_LIBRARIES}
                   PRIVATE_LINK_LIBRARIES GaudiKernel GeoModelInterfaces RDBAccessSvcLib StoreGateLib )
