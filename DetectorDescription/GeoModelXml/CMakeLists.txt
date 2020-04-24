################################################################################
# Package: GeoModelXml
################################################################################

# Declare the package name:
atlas_subdir( GeoModelXml )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          AtlasPolicy
                          PRIVATE
			  DetectorDescription/GeoModelInterfaces
                          Control/StoreGate
                          GaudiKernel )

# External dependencies:
find_package( XercesC REQUIRED )
find_package( Eigen3 REQUIRED )
find_package( GeoModelCore REQUIRED )
find_package( GeoModelTools REQUIRED )
find_package( ZLIB REQUIRED )

# Component(s) in the package:
atlas_add_library( GeoModelXml
                   src/*.cxx
                   PUBLIC_HEADERS GeoModelXml
                   INCLUDE_DIRS ${XERCESC_INCLUDE_DIRS} ${EIGEN3_INCLUDE_DIRS} ${ZLIB_INCLUDE_DIRS} ${GEOMODELCORE_INCLUDE_DIRS} ${GEOMODELTOOLS_INCLUDE_DIRS}
                   LINK_LIBRARIES ${XERCESC_LIBRARIES} ${EIGEN3_LIBRARIES} ${ZLIB_LIBRARIES} ${GEOMODELCORE_LIBRARIES} ${GEOMODELTOOLS_LIBRARIES} StoreGateLib SGtests
                   PRIVATE_LINK_LIBRARIES GaudiKernel )

atlas_install_runtime( data/*.dtd )
