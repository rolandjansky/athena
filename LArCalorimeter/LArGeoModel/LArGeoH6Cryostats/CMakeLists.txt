#Copyright (C) 2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( LArGeoH6Cryostats )

# External dependencies:
find_package( Eigen )
find_package( GeoModelCore )

# Component(s) in the package:
atlas_add_library( LArGeoH6Cryostats
                   src/*.cxx
                   PUBLIC_HEADERS LArGeoH6Cryostats
                   PRIVATE_INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS} ${GEOMODELCORE_INCLUDE_DIRS}
                   LINK_LIBRARIES ${EIGEN_LIBRARIES} ${GEOMODELCORE_LIBRARIES} StoreGateLib SGtests
                   PRIVATE_LINK_LIBRARIES GaudiKernel GeoModelInterfaces RDBAccessSvcLib )

