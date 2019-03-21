################################################################################
# Package: PixelLayoutUtils
################################################################################

# Declare the package name: 
atlas_subdir( PixelLayoutUtils )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          DetectorDescription/GeoModel/GeoModelKernel
                          InnerDetector/InDetDetDescr/PixelGeoModel
                          PRIVATE
                          InnerDetector/InDetDetDescr/InDetGeoModelUtils
                          Tools/PathResolver )

# External dependencies:
find_package( XercesC )

# Component(s) in the package:
atlas_add_library( PixelLayoutUtils
                   src/*.cxx
                   PUBLIC_HEADERS PixelLayoutUtils
                   INCLUDE_DIRS ${XERCESC_INCLUDE_DIRS}
                   LINK_LIBRARIES ${XERCESC_LIBRARIES} InDetGeoModelUtils PathResolver 
		   PRIVATE_LINK_LIBRARIES GeoModelKernel )


