# $Id: CMakeLists.txt 733925 2016-04-05 10:07:31Z krasznaa $
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
   Control/CLIDSvc
   Control/SGTools
   Control/StoreGate
   DetectorDescription/DetDescrCond/DetDescrConditions
   DetectorDescription/GeometryDBSvc
   InnerDetector/InDetConditions/InDetCondServices
   InnerDetector/InDetDetDescr/InDetIdentifier )

# External dependencies:
find_package( CLHEP )

# Check if the GeoModelKernel/GeoTorus.h header is available:
#include_directories(
#   $<TARGET_PROPERTY:PixelGeoModelPkg,INTERFACE_INCLUDE_DIRECTORIES> )
#include( CheckIncludeFiles )
#check_include_files( "GeoModelKernel/GeoTorus.h" HAVE_GEOTORUS_H )

# For now just hardcode that GeoTorus.h is available. But later on some solution
# will have to be found for figuring this out using CMake...

# Component(s) in the package:
atlas_add_component( PixelGeoModel
   PixelGeoModel/*.h src/*.cxx src/components/*.cxx
   INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
   LINK_LIBRARIES ${CLHEP_LIBRARIES} AthenaBaseComps AthenaKernel GeoModelKernel
   GeoModelUtilities Identifier GaudiKernel InDetGeoModelUtils
   InDetReadoutGeometry SGTools StoreGateLib DetDescrConditions
   InDetCondServices InDetIdentifier
   DEFINITIONS "-DGEOTORUS=1" )

# Install files from the package:
atlas_install_headers( PixelGeoModel )
atlas_install_python_modules( python/*.py )
