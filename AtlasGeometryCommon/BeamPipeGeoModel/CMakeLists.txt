################################################################################
# Package: BeamPipeGeoModel
################################################################################

# Declare the package name:
atlas_subdir( BeamPipeGeoModel )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Database/RDBAccessSvc
                          DetectorDescription/GeoModel/GeoModelUtilities
                          PRIVATE
                          Control/SGTools
                          Control/StoreGate
                          Control/AthenaKernel
                          DetectorDescription/GeoModel/GeoModelInterfaces
                          GaudiKernel )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )
find_package( GeoModel )

# Component(s) in the package:
atlas_add_library( BeamPipeGeoModelLib
                   src/*.cxx
                   PUBLIC_HEADERS BeamPipeGeoModel
                   INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS} 
                   LINK_LIBRARIES ${Boost_LIBRARIES} ${CORAL_LIBRARIES} ${GEOMODEL_LIBRARIES} GeoModelUtilities StoreGateLib 
                   PRIVATE_LINK_LIBRARIES SGTools AthenaKernel GaudiKernel )

atlas_add_component( BeamPipeGeoModel
                     src/components/*.cxx
                     INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS}
                     LINK_LIBRARIES ${Boost_LIBRARIES} ${CORAL_LIBRARIES} ${GEOMODEL_LIBRARIES} GeoModelUtilities SGTools StoreGateLib GaudiKernel BeamPipeGeoModelLib )

atlas_add_test( BeamPipeGMConfig_test
                SCRIPT test/BeamPipeGMConfig_test.py
                PROPERTIES TIMEOUT 300 )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_scripts( test/*.py )
