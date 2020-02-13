################################################################################
# Package: GeoModelEnvelopes
################################################################################

# Declare the package name:
atlas_subdir( GeoModelEnvelopes )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaKernel
                          DetectorDescription/GeoModel/GeoModelUtilities
                          PRIVATE
                          Control/SGTools
                          Control/StoreGate
                          DetectorDescription/GeoModel/GeoModelInterfaces
                          GaudiKernel )

find_package( GeoModelCore )

# Component(s) in the package:
atlas_add_component( GeoModelEnvelopes
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES ${GEOMODELCORE_LIBRARIES} GeoModelUtilities SGTools StoreGateLib GaudiKernel )

# Install files from the package:
atlas_install_headers( GeoModelEnvelopes )

