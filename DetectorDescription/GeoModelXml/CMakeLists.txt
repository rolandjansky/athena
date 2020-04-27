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
find_package( XercesC )
find_package( Eigen )
find_package( GeoModelCore )
find_package( GeoModelTools )
find_package( ZLIB )

# Component(s) in the package:
atlas_add_library( GeoModelXml
                   src/*.cxx
                   PUBLIC_HEADERS GeoModelXml
                   INCLUDE_DIRS ${XERCESC_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS} ${ZLIB_INCLUDE_DIRS} ${GEOMODELCORE_INCLUDE_DIRS} ${GEOMODELTOOLS_INCLUDE_DIRS}
                   LINK_LIBRARIES ${XERCESC_LIBRARIES} ${EIGEN_LIBRARIES} ${ZLIB_LIBRARIES} ${GEOMODELCORE_LIBRARIES} ${GEOMODELTOOLS_LIBRARIES} StoreGateLib SGtests
                   PRIVATE_LINK_LIBRARIES GaudiKernel )

atlas_install_runtime( data/*.dtd )
