################################################################################
# Package: SCT_GeoModel
################################################################################

# Declare the package name:
atlas_subdir( SCT_GeoModel )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaKernel
                          Database/AthenaPOOL/RDBAccessSvc
                          DetectorDescription/GeoModel/GeoModelKernel
                          DetectorDescription/GeoModel/GeoModelUtilities
                          GaudiKernel
                          InnerDetector/InDetDetDescr/InDetGeoModelUtils
                          InnerDetector/InDetDetDescr/InDetReadoutGeometry
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
                          InnerDetector/InDetDetDescr/InDetIdentifier )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( CLHEP )
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )

# Component(s) in the package:
atlas_add_library( SCT_GeoModelLib
                   src/*.cxx
                   PUBLIC_HEADERS SCT_GeoModel
                   INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                   DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${Boost_LIBRARIES} ${CORAL_LIBRARIES} ${CLHEP_LIBRARIES} AthenaKernel GeoModelKernel GeoModelUtilities GaudiKernel InDetGeoModelUtils InDetReadoutGeometry StoreGateLib SGtests
                   PRIVATE_LINK_LIBRARIES SGTools AthenaPoolUtilities DetDescrConditions Identifier InDetCondServices InDetIdentifier )

atlas_add_component( SCT_GeoModel
                     src/components/*.cxx
                     INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${Boost_LIBRARIES} ${CORAL_LIBRARIES} ${CLHEP_LIBRARIES} AthenaKernel GeoModelKernel GeoModelUtilities GaudiKernel InDetGeoModelUtils InDetReadoutGeometry SGTools StoreGateLib SGtests AthenaPoolUtilities DetDescrConditions Identifier InDetCondServices InDetIdentifier SCT_GeoModelLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )

