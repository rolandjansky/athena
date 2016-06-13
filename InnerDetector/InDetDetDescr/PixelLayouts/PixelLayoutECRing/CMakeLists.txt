################################################################################
# Package: PixelLayoutECRing
################################################################################

# Declare the package name:
atlas_subdir( PixelLayoutECRing )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          DetectorDescription/GeoModel/GeoModelKernel
                          GaudiKernel
                          InnerDetector/InDetDetDescr/InDetTrackingGeometryXML
                          InnerDetector/InDetDetDescr/PixelGeoModel
                          InnerDetector/InDetDetDescr/PixelLayouts/PixelInterfaces
                          InnerDetector/InDetDetDescr/PixelLayouts/PixelLayoutUtils
                          PRIVATE
                          DetectorDescription/Identifier
                          InnerDetector/InDetDetDescr/InDetGeoModelUtils
                          InnerDetector/InDetDetDescr/InDetIdentifier
                          InnerDetector/InDetDetDescr/InDetReadoutGeometry
                          InnerDetector/InDetDetDescr/PixelLayouts/PixelGeoModelModule
                          Tools/PathResolver )

# External dependencies:
find_package( XercesC )

# Component(s) in the package:
atlas_add_component( PixelLayoutECRing
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${XERCESC_INCLUDE_DIRS}
                     LINK_LIBRARIES ${XERCESC_LIBRARIES} 
		     AthenaBaseComps AthenaKernel GeoModelKernel GaudiKernel
		     PixelGeoModelModule PixelLayoutUtils
		     PRIVATE_LINK_LIBRARIES Identifier InDetGeoModelUtils InDetIdentifier InDetReadoutGeometry PathResolver )

# Install files from the package:
atlas_install_headers( PixelLayoutECRing )

