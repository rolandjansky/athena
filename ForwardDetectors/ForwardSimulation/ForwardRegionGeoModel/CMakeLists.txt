################################################################################
# Package: ForwardRegionGeoModel
################################################################################

# Declare the package name:
atlas_subdir( ForwardRegionGeoModel )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaKernel
                          DetectorDescription/GeoModel/GeoModelUtilities
                          GaudiKernel
                          PRIVATE
                          Control/StoreGate
                          DetectorDescription/GeoModel/GeoModelInterfaces
                          Tools/PathResolver )

# External dependencies:
find_package( CLHEP )
find_package( Eigen )
find_package( GeoModelCore )

# Component(s) in the package:
atlas_add_library( ForwardRegionGeoModelLib
                   src/*.cxx
                   PUBLIC_HEADERS ForwardRegionGeoModel
                   INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                   DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${CLHEP_LIBRARIES} ${EIGEN_LIBRARIES} ${GEOMODELCORE_LIBRARIES} AthenaKernel GeoModelUtilities GaudiKernel StoreGateLib SGtests
                   PRIVATE_LINK_LIBRARIES PathResolver )

atlas_add_component( ForwardRegionGeoModel
                     src/components/*.cxx
                     INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} ${EIGEN_LIBRARIES} ${GEOMODELCORE_LIBRARIES} GeoModelUtilities GaudiKernel StoreGateLib SGtests PathResolver ForwardRegionGeoModelLib )

# Install files from the package:
atlas_install_runtime( share/*.csv )

