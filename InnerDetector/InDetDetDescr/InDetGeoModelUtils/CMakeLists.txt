################################################################################
# Package: InDetGeoModelUtils
################################################################################

# Declare the package name:
atlas_subdir( InDetGeoModelUtils )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaKernel
                          Database/AthenaPOOL/RDBAccessSvc
                          DetectorDescription/GeoModel/GeoModelKernel
                          GaudiKernel
                          PRIVATE
                          Control/StoreGate
                          DetectorDescription/GeoModel/GeoModelInterfaces
                          DetectorDescription/GeoModel/GeoModelUtilities
                          DetectorDescription/GeometryDBSvc )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( CLHEP )
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )

# Component(s) in the package:
atlas_add_library( InDetGeoModelUtils
                   src/*.cxx
                   PUBLIC_HEADERS InDetGeoModelUtils
                   INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                   DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${Boost_LIBRARIES} ${CORAL_LIBRARIES} ${CLHEP_LIBRARIES} AthenaKernel GeoModelKernel GaudiKernel StoreGateLib SGtests
                   PRIVATE_LINK_LIBRARIES GeoModelUtilities )

