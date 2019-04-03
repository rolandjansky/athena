################################################################################
# Package: GeoModelXml
################################################################################

# Declare the package name:
atlas_subdir( GeoModelXml )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          AtlasPolicy
                          DetectorDescription/GeoModel/GeoModelKernel
                          PRIVATE
                          Control/StoreGate
                          DetectorDescription/GeoModel/GeoModelInterfaces
                          GaudiKernel )

# External dependencies:
find_package( CLHEP )
find_package( XercesC )
find_package( ZLIB )

# Component(s) in the package:
atlas_add_library( GeoModelXml
                   src/*.cxx
                   PUBLIC_HEADERS GeoModelXml
                   INCLUDE_DIRS ${XERCESC_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${ZLIB_INCLUDE_DIRS}
                   DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${XERCESC_LIBRARIES} ${CLHEP_LIBRARIES} ${ZLIB_LIBRARIES} GeoModelKernel StoreGateLib SGtests
                   PRIVATE_LINK_LIBRARIES GaudiKernel )

atlas_install_runtime( data/*.dtd )
