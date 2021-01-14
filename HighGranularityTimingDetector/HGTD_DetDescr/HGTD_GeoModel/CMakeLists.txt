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
                          HighGranularityTimingDetector/HGTD_DetDescr/HGTD_Identifier
                          HighGranularityTimingDetector/HGTD_DetDescr/HGTD_ReadoutGeometry
                          PRIVATE
                          Control/SGTools
                          Control/StoreGate
                          Database/AthenaPOOL/AthenaPoolUtilities
                          DetectorDescription/DetDescrCond/DetDescrConditions
                          DetectorDescription/ReadoutGeometryBase
                          DetectorDescription/GeoModel/GeoModelInterfaces
                          DetectorDescription/GeometryDBSvc
                          DetectorDescription/Identifier )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( CLHEP )
find_package( Eigen )
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )

# Component(s) in the package:
atlas_add_library( HGTD_GeoModelLib
                   src/*.cxx
                   PUBLIC_HEADERS HGTD_GeoModel
                   INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                   DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${Boost_LIBRARIES} ${CORAL_LIBRARIES} ${CLHEP_LIBRARIES} AthenaKernel GeoModelUtilities GaudiKernel InDetGeoModelUtils HGTD_Identifier HGTD_ReadoutGeometry ReadoutGeometryBase StoreGateLib SGtests
                   PRIVATE_LINK_LIBRARIES SGTools AthenaPoolUtilities DetDescrConditions Identifier )

atlas_add_component( HGTD_GeoModel
                     src/components/*.cxx
                     INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${Boost_LIBRARIES} ${CORAL_LIBRARIES} ${CLHEP_LIBRARIES} AthenaKernel GeoModelUtilities GaudiKernel InDetGeoModelUtils HGTD_Identifier HGTD_ReadoutGeometry SGTools AthenaPoolUtilities DetDescrConditions StoreGateLib SGtests Identifier HGTD_GeoModelLib )
