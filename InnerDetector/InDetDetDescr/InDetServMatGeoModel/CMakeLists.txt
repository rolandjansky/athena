################################################################################
# Package: InDetServMatGeoModel
################################################################################

# Declare the package name:
atlas_subdir( InDetServMatGeoModel )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Database/AthenaPOOL/RDBAccessSvc
                          DetectorDescription/GeoModel/GeoModelKernel
                          DetectorDescription/GeoModel/GeoModelUtilities
                          GaudiKernel
                          InnerDetector/InDetDetDescr/InDetGeoModelUtils
                          PRIVATE
                          Control/CLIDSvc
                          Control/SGTools
                          Control/StoreGate
                          DetectorDescription/GeoModel/GeoModelInterfaces
                          DetectorDescription/GeometryDBSvc )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( CLHEP )
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )

# Component(s) in the package:
atlas_add_component( InDetServMatGeoModel
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${Boost_LIBRARIES} ${CORAL_LIBRARIES} ${CLHEP_LIBRARIES} AthenaBaseComps AthenaKernel GeoModelKernel GeoModelUtilities GaudiKernel InDetGeoModelUtils SGTools StoreGateLib SGtests )

# Install files from the package:
atlas_install_headers( InDetServMatGeoModel )

