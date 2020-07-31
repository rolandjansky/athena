################################################################################
# Package: LArGeoFcal
################################################################################

# Declare the package name:
atlas_subdir( LArGeoFcal )

# External dependencies:
find_package( Eigen )
find_package( GeoModelCore )

# tag NEEDS_CORAL_BASE was not recognized in automatic conversion in cmt2cmake

# Component(s) in the package:
atlas_add_library( LArGeoFcal
                   src/*.cxx
                   PUBLIC_HEADERS LArGeoFcal
                   INCLUDE_DIRS ${GEOMODELCORE_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS}
                   LINK_LIBRARIES ${GEOMODELCORE_LIBRARIES} StoreGateLib SGtests
                   PRIVATE_LINK_LIBRARIES ${EIGEN_LIBRARIES} GeoModelUtilities GaudiKernel LArReadoutGeometry RDBAccessSvcLib  )

