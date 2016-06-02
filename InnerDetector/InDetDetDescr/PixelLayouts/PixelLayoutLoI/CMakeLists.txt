################################################################################
# Package: PixelLayoutLoI
################################################################################

# Declare the package name:
atlas_subdir( PixelLayoutLoI )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          DetectorDescription/GeoModel/GeoModelKernel
                          GaudiKernel
                          InnerDetector/InDetDetDescr/InDetTrackingGeometryXML
                          InnerDetector/InDetDetDescr/PixelGeoModel
                          InnerDetector/InDetDetDescr/PixelLayouts/PixelGeoModelModule
                          InnerDetector/InDetDetDescr/PixelLayouts/PixelInterfaces
                          InnerDetector/InDetDetDescr/PixelLayouts/PixelLayoutUtils
                          PRIVATE
                          DetectorDescription/GeoModel/GeoModelInterfaces
                          DetectorDescription/Identifier
                          InnerDetector/InDetDetDescr/InDetGeoModelUtils
                          InnerDetector/InDetDetDescr/InDetIdentifier
                          InnerDetector/InDetDetDescr/InDetReadoutGeometry
                          Tools/PathResolver )

# External dependencies:
find_package( CLHEP )
find_package( XercesC )

# Component(s) in the package:
atlas_add_component( PixelLayoutLoI
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${XERCESC_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${XERCESC_LIBRARIES} ${CLHEP_LIBRARIES} 
		     AthenaBaseComps AthenaKernel GeoModelKernel GaudiKernel
		     PixelGeoModelModule PixelLayoutUtils
		     PRIVATE_LINK_LIBRARIES Identifier InDetGeoModelUtils InDetIdentifier InDetReadoutGeometry PathResolver )

# Install files from the package:
atlas_install_headers( PixelLayoutLoI )

