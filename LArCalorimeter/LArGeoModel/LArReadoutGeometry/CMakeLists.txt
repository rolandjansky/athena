################################################################################
# Package: LArReadoutGeometry
################################################################################

# Declare the package name:
atlas_subdir( LArReadoutGeometry )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( CLHEP )
find_package( Eigen )
find_package( GeoModelCore )

# Component(s) in the package:
atlas_add_library( LArReadoutGeometry
                   src/*.cxx
                   PUBLIC_HEADERS LArReadoutGeometry
                   INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                   PRIVATE_DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${EIGEN_LIBRARIES} ${GEOMODELCORE_LIBRARIES} AthenaKernel GeoPrimitives Identifier LArHV StoreGateLib SGtests
                   PRIVATE_LINK_LIBRARIES ${Boost_LIBRARIES} ${CLHEP_LIBRARIES} GeoModelUtilities GaudiKernel RDBAccessSvcLib GeoModelInterfaces )

