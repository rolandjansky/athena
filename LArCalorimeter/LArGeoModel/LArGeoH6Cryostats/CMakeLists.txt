#Copyright (C) 2021 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( LArGeoH6Cryostats )

# External dependencies:
find_package( Eigen )
find_package( GeoModel COMPONENTS GeoModelKernel GeoGenericFunctions )

# Component(s) in the package:
atlas_add_library( LArGeoH6Cryostats
                   src/*.cxx
                   PUBLIC_HEADERS LArGeoH6Cryostats
                   PRIVATE_INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS} ${GEOMODEL_INCLUDE_DIRS}
                   LINK_LIBRARIES ${EIGEN_LIBRARIES} ${GEOMODEL_LIBRARIES} StoreGateLib SGtests
                   PRIVATE_LINK_LIBRARIES GaudiKernel GeoModelInterfaces RDBAccessSvcLib )

