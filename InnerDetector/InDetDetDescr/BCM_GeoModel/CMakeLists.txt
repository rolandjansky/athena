################################################################################
# Package: BCM_GeoModel
################################################################################

# Declare the package name:
atlas_subdir( BCM_GeoModel )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          DetectorDescription/GeoModel/GeoModelInterfaces
                          GaudiKernel
                          PRIVATE
                          Database/RDBAccessSvc
                          DetectorDescription/GeoModel/GeoModelUtilities )

# External dependencies:
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )
find_package( Eigen )
find_package( GeoModelCore )

# Component(s) in the package:
atlas_add_component( BCM_GeoModel
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${CORAL_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CORAL_LIBRARIES} ${GEOMODELCORE_LIBRARIES} AthenaBaseComps AthenaKernel GaudiKernel GeoModelUtilities RDBAccessSvcLib )

# Install files from the package:
atlas_install_headers( BCM_GeoModel )

