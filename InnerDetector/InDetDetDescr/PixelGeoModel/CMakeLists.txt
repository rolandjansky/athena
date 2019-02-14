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
   Database/RDBAccessSvc
   DetectorDescription/GeoModel/GeoModelInterfaces
   DetectorDescription/GeoModel/GeoModelUtilities
   DetectorDescription/Identifier
   DetectorDescription/GeoPrimitives
   GaudiKernel
   InnerDetector/InDetDetDescr/InDetGeoModelUtils
   InnerDetector/InDetDetDescr/InDetReadoutGeometry
   PRIVATE
   Control/SGTools
   Control/StoreGate
   DetectorDescription/DetDescrCond/DetDescrConditions
   DetectorDescription/GeometryDBSvc
   InnerDetector/InDetDetDescr/InDetIdentifier )

# External dependencies:
find_package( Eigen )
find_package( GeoModel )

# Component(s) in the package:
atlas_add_library( PixelGeoModelLib
   PixelGeoModel/*.h src/*.cxx
   PUBLIC_HEADERS PixelGeoModel
   INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS}
   LINK_LIBRARIES ${EIGEN_LIBRARIES} ${GEOMODEL_LIBRARIES} AthenaBaseComps AthenaKernel 
   GeoModelUtilities Identifier GaudiKernel InDetGeoModelUtils
   InDetReadoutGeometry
   PRIVATE_LINK_LIBRARIES SGTools StoreGateLib DetDescrConditions
   InDetIdentifier
   PRIVATE_DEFINITIONS "-DGEOTORUS=1" )

atlas_add_component( PixelGeoModel
   src/components/*.cxx
   LINK_LIBRARIES GaudiKernel PixelGeoModelLib )

atlas_add_test( PixelGMConfig_test
                SCRIPT test/PixelGMConfig_test.py
                PROPERTIES TIMEOUT 300 )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_scripts( test/*.py )
