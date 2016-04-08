################################################################################
# Package: LArGeoTBBarrel
################################################################################

# Declare the package name:
atlas_subdir( LArGeoTBBarrel )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          DetectorDescription/GeoModel/GeoModelKernel
                          PRIVATE
                          Control/StoreGate
                          DetectorDescription/GeoModel/GeoModelInterfaces
                          DetectorDescription/GeoModel/GeoModelUtilities
                          GaudiKernel
                          LArCalorimeter/LArGeoModel/LArGeoBarrel )

# External dependencies:
find_package( CLHEP )

# Component(s) in the package:
atlas_add_library( LArGeoTBBarrel
                   src/*.cxx
                   PUBLIC_HEADERS LArGeoTBBarrel
                   PRIVATE_INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                   PRIVATE_DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES GeoModelKernel StoreGateLib SGtests
                   PRIVATE_LINK_LIBRARIES ${CLHEP_LIBRARIES} GeoModelUtilities GaudiKernel LArGeoBarrel )

