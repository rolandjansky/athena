################################################################################
# Package: SCT_SLHC_GeoModel
################################################################################

# Declare the package name:
atlas_subdir( SCT_SLHC_GeoModel )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Database/AthenaPOOL/RDBAccessSvc
                          DetectorDescription/GeoModel/GeoModelUtilities
                          DetectorDescription/Identifier
                          GaudiKernel
                          InnerDetector/InDetDetDescr/InDetGeoModelUtils
                          InnerDetector/InDetDetDescr/InDetReadoutGeometry
                          PRIVATE
                          Control/CLIDSvc
                          Control/SGTools
                          Control/StoreGate
                          DetectorDescription/GeoModel/GeoModelInterfaces
                          DetectorDescription/GeoModel/GeoModelKernel
                          DetectorDescription/GeometryDBSvc
                          InnerDetector/InDetConditions/InDetCondServices
                          InnerDetector/InDetDetDescr/InDetIdentifier )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( CLHEP )
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )

# Component(s) in the package:
atlas_add_library( SCT_SLHC_GeoModelLib
                   src/*.cxx
                   PUBLIC_HEADERS SCT_SLHC_GeoModel
                   INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                   DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${Boost_LIBRARIES} ${CORAL_LIBRARIES} ${CLHEP_LIBRARIES} GeoModelUtilities Identifier GaudiKernel InDetGeoModelUtils InDetReadoutGeometry StoreGateLib SGtests
                   PRIVATE_LINK_LIBRARIES SGTools GeoModelKernel InDetCondServices InDetIdentifier )

atlas_add_component( SCT_SLHC_GeoModel
                     src/components/*.cxx
                     INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${Boost_LIBRARIES} ${CORAL_LIBRARIES} ${CLHEP_LIBRARIES} GeoModelUtilities Identifier GaudiKernel InDetGeoModelUtils InDetReadoutGeometry SGTools StoreGateLib SGtests GeoModelKernel InDetCondServices InDetIdentifier SCT_SLHC_GeoModelLib )

