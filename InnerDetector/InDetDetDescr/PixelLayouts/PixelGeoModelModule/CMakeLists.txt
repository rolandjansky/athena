################################################################################
# Package: PixelGeoModelModule
################################################################################

# Declare the package name:
atlas_subdir( PixelGeoModelModule )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          DetectorDescription/Identifier
                          InnerDetector/InDetDetDescr/PixelLayouts/PixelLayoutUtils
                          PRIVATE
                          DetectorDescription/GeoModel/GeoModelKernel
                          InnerDetector/InDetDetDescr/InDetGeoModelUtils
                          InnerDetector/InDetDetDescr/InDetIdentifier
                          InnerDetector/InDetDetDescr/InDetReadoutGeometry
                          InnerDetector/InDetDetDescr/PixelGeoModel
                          InnerDetector/InDetDetDescr/PixelLayouts/PixelGeoComponent
                          Tools/PathResolver )

# External dependencies:
find_package( XercesC )

# Component(s) in the package:
atlas_add_library( PixelGeoModelModule
                   src/*.cxx
                   PUBLIC_HEADERS PixelGeoModelModule
                   INCLUDE_DIRS ${XERCESC_INCLUDE_DIRS}
                   LINK_LIBRARIES ${XERCESC_LIBRARIES} 
		   GeoModelKernel InDetGeoModelUtils InDetIdentifier 
		   InDetReadoutGeometry PathResolver PixelGeoComponent
		   PRIVATE_LINK_LIBRARIES Identifier PixelLayoutUtils )

