################################################################################
# Package: CavernInfraGeoModel
################################################################################

# Declare the package name:
atlas_subdir( CavernInfraGeoModel )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/CLIDSvc
                          Control/SGTools
                          Database/AthenaPOOL/RDBAccessSvc
                          DetectorDescription/GeoModel/GeoModelKernel
                          DetectorDescription/GeoModel/GeoModelUtilities
                          PRIVATE
                          Control/StoreGate
                          DetectorDescription/GeoModel/GeoModelInterfaces
                          GaudiKernel )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( CLHEP )
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )

# Component(s) in the package:
atlas_add_component( CavernInfraGeoModel
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${Boost_LIBRARIES} ${CORAL_LIBRARIES} ${CLHEP_LIBRARIES} SGTools GeoModelKernel GeoModelUtilities StoreGateLib SGtests GaudiKernel )

# Install files from the package:
atlas_install_headers( CavernInfraGeoModel )

