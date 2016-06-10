################################################################################
# Package: PixelSCTDBHelper
################################################################################

# Declare the package name:
atlas_subdir( PixelSCTDBHelper )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaKernel
                          Database/AthenaPOOL/RDBAccessSvc
                          InnerDetector/InDetDetDescr/InDetGeoModelUtils
                          InnerDetector/InDetDetDescr/PixelGeoModel
                          PRIVATE
                          DetectorDescription/GeoModel/GeoModelKernel )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( CLHEP )
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )

# Component(s) in the package:
atlas_add_library( PixelSCTDBHelper
                   src/*.cxx
                   PUBLIC_HEADERS PixelSCTDBHelper
                   INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                   DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${Boost_LIBRARIES} ${CORAL_LIBRARIES} ${CLHEP_LIBRARIES}
		   AthenaKernel	RDBAccessSvc InDetGeoModelUtils GeoModelKernel )

