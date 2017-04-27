################################################################################
# Package: PixelGeoModel
################################################################################

# Declare the package name:
atlas_subdir( PixelGeoModel )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   Control/AthenaBaseComps
   Control/AthenaKernel
   Database/AthenaPOOL/RDBAccessSvc
   DetectorDescription/GeoModel/GeoModelInterfaces
   DetectorDescription/GeoModel/GeoModelKernel
   DetectorDescription/GeoModel/GeoModelUtilities
   DetectorDescription/Identifier
   GaudiKernel
   InnerDetector/InDetDetDescr/InDetGeoModelUtils
   InnerDetector/InDetDetDescr/InDetReadoutGeometry
   PRIVATE
   Control/SGTools
   Control/StoreGate
   DetectorDescription/DetDescrCond/DetDescrConditions
   DetectorDescription/GeometryDBSvc
   InnerDetector/InDetConditions/InDetCondServices
   InnerDetector/InDetDetDescr/InDetIdentifier )

# External dependencies:
find_package( CLHEP )

# Component(s) in the package:
atlas_add_library( PixelGeoModelLib
   PixelGeoModel/*.h src/*.cxx
   PUBLIC_HEADERS PixelGeoModel
   INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
   LINK_LIBRARIES ${CLHEP_LIBRARIES} AthenaBaseComps AthenaKernel GeoModelKernel
   GeoModelUtilities Identifier GaudiKernel InDetGeoModelUtils
   InDetReadoutGeometry
   PRIVATE_LINK_LIBRARIES SGTools StoreGateLib DetDescrConditions
   InDetCondServices InDetIdentifier
   PRIVATE_DEFINITIONS "-DGEOTORUS=1" )

atlas_add_component( PixelGeoModel
   src/components/*.cxx
   LINK_LIBRARIES GaudiKernel PixelGeoModelLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )
