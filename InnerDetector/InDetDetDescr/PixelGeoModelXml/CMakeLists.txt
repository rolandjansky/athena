################################################################################
# Package: PixelGeoModelXml
################################################################################

# Declare the package name:
atlas_subdir( PixelGeoModelXml )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaKernel
                          Database/AthenaPOOL/RDBAccessSvc
                          DetectorDescription/GeoModelXml
			  DetectorDescription/GeoModel/GeoModelUtilities
                          GaudiKernel
                          InnerDetector/InDetDetDescr/InDetGeoModelUtils
                          InnerDetector/InDetDetDescr/PixelReadoutGeometry
                          InnerDetector/InDetDetDescr/PixelReadoutGeometry
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
                          InnerDetector/InDetDetDescr/InDetIdentifier
			  Tools/PathResolver )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )

# Component(s) in the package:
atlas_add_library( PixelGeoModelXmlLib
                   src/*.cxx
                   PUBLIC_HEADERS PixelGeoModelXml
                   INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS} 
                   LINK_LIBRARIES ${Boost_LIBRARIES} ${CORAL_LIBRARIES} AthenaKernel GeoModelXml GeoModelUtilities GaudiKernel InDetGeoModelUtils InDetReadoutGeometry PixelReadoutGeometry PixelReadoutGeometry StoreGateLib InDetSimEvent
                   PRIVATE_LINK_LIBRARIES SGTools AthenaPoolUtilities DetDescrConditions Identifier InDetIdentifier PathResolver)

atlas_add_component( PixelGeoModelXml
                     src/components/*.cxx
                     INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${Boost_LIBRARIES} ${CORAL_LIBRARIES} ${CLHEP_LIBRARIES} AthenaKernel GeoModelXml GeoModelUtilities GaudiKernel InDetGeoModelUtils InDetReadoutGeometry PixelReadoutGeometry PixelReadoutGeometry SGTools StoreGateLib SGtests AthenaPoolUtilities DetDescrConditions Identifier InDetIdentifier PixelGeoModelXmlLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )
