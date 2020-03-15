################################################################################
# Package: HGTD_GeoModel
################################################################################

# Declare the package name:
atlas_subdir( HGTD_GeoModel )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaKernel
                          DetectorDescription/GeoModel/GeoModelUtilities
                          GaudiKernel
                          InnerDetector/InDetDetDescr/InDetGeoModelUtils
                          HighGranularityTimingDetector/HGTD_DetDescr/HGTD_GeoModel/HGTD_ReadoutGeometry
                          PRIVATE
                          Control/SGTools
                          Control/StoreGate
                          DetectorDescription/GeoModel/GeoModelInterfaces
                          DetectorDescription/GeometryDBSvc
                          DetectorDescription/Identifier )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( CLHEP )
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )

# Component(s) in the package:
atlas_add_library( HGTD_GeoModelLib
                   src/*.cxx
                   PUBLIC_HEADERS HGTD_GeoModel
                   INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                   DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${Boost_LIBRARIES} ${CORAL_LIBRARIES} ${CLHEP_LIBRARIES} AthenaKernel GeoModelUtilities GaudiKernel InDetGeoModelUtils HGTD_ReadoutGeometry StoreGateLib SGtests
                   PRIVATE_LINK_LIBRARIES SGTools Identifier )

atlas_add_component( HGTD_GeoModel
                     src/components/*.cxx
                     INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${Boost_LIBRARIES} ${CORAL_LIBRARIES} ${CLHEP_LIBRARIES} AthenaKernel GeoModelUtilities GaudiKernel InDetGeoModelUtils HGTD_ReadoutGeometry SGTools StoreGateLib SGtests Identifier HGTD_GeoModelLib )
