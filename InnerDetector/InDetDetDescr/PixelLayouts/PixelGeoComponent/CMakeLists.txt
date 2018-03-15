################################################################################
# Package: PixelGeoComponent
################################################################################

# Declare the package name:
atlas_subdir( PixelGeoComponent )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/SGTools
                          DetectorDescription/GeoModel/GeoModelKernel
                          PRIVATE
                          InnerDetector/InDetDetDescr/InDetGeoModelUtils )

# External dependencies:
find_package( CLHEP )

# Component(s) in the package:
atlas_add_library( PixelGeoComponent
                   src/*.cxx
                   PUBLIC_HEADERS PixelGeoComponent
                   INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                   DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${CLHEP_LIBRARIES} InDetGeoModelUtils 
		   PRIVATE_LINK_LIBRARIES ${CLHEP_LIBRARIES} SGTools GeoModelKernel )

