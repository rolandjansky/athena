################################################################################
# Package: LArGeoHec
################################################################################

# Declare the package name:
atlas_subdir( LArGeoHec )

# External dependencies:
find_package( Eigen )
find_package( GeoModelCore )

# Component(s) in the package:
atlas_add_library( LArGeoHec
                   src/*.cxx
                   PUBLIC_HEADERS LArGeoHec
                   PRIVATE_INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS} ${GEOMODELCORE_INCLUDE_DIRS}
                   LINK_LIBRARIES StoreGateLib SGtests
                   PRIVATE_LINK_LIBRARIES ${EIGEN_LIBRARIES} ${GEOMODELCORE_LIBRARIES} AthenaKernel GeoModelUtilities GaudiKernel RDBAccessSvcLib )

