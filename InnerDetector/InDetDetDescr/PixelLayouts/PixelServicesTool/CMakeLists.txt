################################################################################
# Package: PixelServicesTool
################################################################################

# Declare the package name:
atlas_subdir( PixelServicesTool )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Database/AthenaPOOL/RDBAccessSvc
                          GaudiKernel
                          InnerDetector/InDetDetDescr/InDetGeoModelUtils
                          InnerDetector/InDetDetDescr/InDetTrackingGeometryXML
                          InnerDetector/InDetDetDescr/PixelGeoModel
                          InnerDetector/InDetDetDescr/PixelLayouts/PixelGeoModelModule
                          InnerDetector/InDetDetDescr/PixelLayouts/PixelInterfaces
                          InnerDetector/InDetDetDescr/PixelLayouts/PixelLayoutUtils
                          PRIVATE
                          DetectorDescription/GeoModel/GeoModelKernel
                          DetectorDescription/GeometryDBSvc
                          InnerDetector/InDetDetDescr/PixelLayouts/PixelSCTDBHelper
                          Tools/PathResolver )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( CLHEP )
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )

# Component(s) in the package:
atlas_add_component( PixelServicesTool
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${Boost_LIBRARIES} ${CORAL_LIBRARIES} ${CLHEP_LIBRARIES}
		     AthenaBaseComps AthenaKernel RDBAccessSvc GaudiKernel InDetGeoModelUtils
		     PixelGeoModelModule PixelLayoutUtils 
		     PRIVATE_LINK_LIBRARIES GeoModelKernel PixelSCTDBHelper PathResolver )

# Install files from the package:
atlas_install_headers( PixelServicesTool )

