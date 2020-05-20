################################################################################
# Package: SCT_GeoModelXml
################################################################################

# Declare the package name:
atlas_subdir( SCT_GeoModelXml )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaKernel
                          Database/AthenaPOOL/RDBAccessSvc
                          DetectorDescription/GeoModelXml
                          DetectorDescription/ReadoutGeometryBase
			  DetectorDescription/GeoModel/GeoModelUtilities
                          GaudiKernel
                          InnerDetector/InDetDetDescr/InDetGeoModelUtils
                          InnerDetector/InDetDetDescr/SCT_ReadoutGeometry
                          InnerDetector/InDetDetDescr/SCT_ReadoutGeometry
                          InnerDetector/InDetDetDescr/InDetReadoutGeometry
			  InnerDetector/InDetSimEvent
                          PRIVATE
                          Control/CLIDSvc
                          Control/SGTools
                          Control/StoreGate
                          Database/AthenaPOOL/AthenaPoolUtilities
                          DetectorDescription/DetDescrCond/DetDescrConditions
                          DetectorDescription/GeoModel/GeoModelInterfaces
                          DetectorDescription/GeometryDBSvc
                          DetectorDescription/Identifier
                          InnerDetector/InDetConditions/InDetCondServices
                          InnerDetector/InDetDetDescr/InDetIdentifier
			  Tools/PathResolver )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( CLHEP )
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )

# Component(s) in the package:
atlas_add_library( SCT_GeoModelXmlLib
                   src/*.cxx
                   PUBLIC_HEADERS SCT_GeoModelXml
                   INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                   DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${Boost_LIBRARIES} ${CORAL_LIBRARIES} ${CLHEP_LIBRARIES} AthenaKernel GeoModelXml GeoModelUtilities ReadoutGeometryBase GaudiKernel InDetGeoModelUtils InDetReadoutGeometry SCT_ReadoutGeometry SCT_ReadoutGeometry StoreGateLib SGtests InDetSimEvent
                   PRIVATE_LINK_LIBRARIES SGTools AthenaPoolUtilities DetDescrConditions Identifier InDetCondServices InDetIdentifier PathResolver)

atlas_add_component( SCT_GeoModelXml
                     src/components/*.cxx
                     INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${Boost_LIBRARIES} ${CORAL_LIBRARIES} ${CLHEP_LIBRARIES} AthenaKernel GeoModelXml GeoModelUtilities ReadoutGeometryBase GaudiKernel InDetGeoModelUtils InDetReadoutGeometry SCT_ReadoutGeometry SCT_ReadoutGeometry SGTools StoreGateLib SGtests AthenaPoolUtilities DetDescrConditions Identifier InDetCondServices InDetIdentifier SCT_GeoModelXmlLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )


