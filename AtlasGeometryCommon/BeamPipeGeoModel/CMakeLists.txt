################################################################################
# Package: BeamPipeGeoModel
################################################################################

# Declare the package name:
atlas_subdir( BeamPipeGeoModel )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Database/AthenaPOOL/RDBAccessSvc
                          DetectorDescription/GeoModel/GeoModelKernel
                          DetectorDescription/GeoModel/GeoModelUtilities
                          PRIVATE
                          Control/CLIDSvc
                          Control/SGTools
                          Control/StoreGate
                          DetectorDescription/GeoModel/GeoModelInterfaces
                          GaudiKernel )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )

# Component(s) in the package:
atlas_add_library( BeamPipeGeoModelLib
                   src/*.cxx
                   PUBLIC_HEADERS BeamPipeGeoModel
                   INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS}
                   LINK_LIBRARIES ${Boost_LIBRARIES} ${CORAL_LIBRARIES} GeoModelKernel GeoModelUtilities StoreGateLib SGtests
                   PRIVATE_LINK_LIBRARIES SGTools GaudiKernel )

atlas_add_component( BeamPipeGeoModel
                     src/components/*.cxx
                     INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS}
                     LINK_LIBRARIES ${Boost_LIBRARIES} ${CORAL_LIBRARIES} GeoModelKernel GeoModelUtilities SGTools StoreGateLib SGtests GaudiKernel BeamPipeGeoModelLib )

