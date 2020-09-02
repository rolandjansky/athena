################################################################################
# Package: InDetServMatGeoModel
################################################################################

# Declare the package name:
atlas_subdir( InDetServMatGeoModel )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Control/CxxUtils
                          Database/RDBAccessSvc
                          DetectorDescription/GeoPrimitives
                          DetectorDescription/GeoModel/GeoModelUtilities
                          GaudiKernel
                          InnerDetector/InDetDetDescr/InDetGeoModelUtils
                          PRIVATE
                          Control/AthenaKernel
                          Control/SGTools
                          Control/StoreGate
                          DetectorDescription/GeoModel/GeoModelInterfaces
                          DetectorDescription/GeometryDBSvc )

# External dependencies:
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )
find_package( Eigen )
find_package( GeoModelCore )

# Component(s) in the package:
atlas_add_component( InDetServMatGeoModel
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${CORAL_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CORAL_LIBRARIES} ${GEOMODELCORE_LIBRARIES} AthenaBaseComps AthenaKernel CxxUtils GeoModelUtilities GaudiKernel InDetGeoModelUtils SGTools StoreGateLib SGtests GeometryDBSvcLib )

# Install files from the package:
atlas_install_headers( InDetServMatGeoModel )

