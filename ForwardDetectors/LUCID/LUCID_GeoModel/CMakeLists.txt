################################################################################
# Package: LUCID_GeoModel
################################################################################

# Declare the package name:
atlas_subdir( LUCID_GeoModel )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          DetectorDescription/GeoModel/GeoModelUtilities
                          GaudiKernel
                          PRIVATE
                          Control/AthenaKernel
                          Control/SGTools
                          Control/StoreGate
                          Database/RDBAccessSvc
                          DetectorDescription/GeoModel/GeoModelInterfaces )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )
find_package( Eigen )
find_package( GeoModelCore )

# Component(s) in the package:
atlas_add_library( LUCID_GeoModelLib
                   src/*.cxx
                   PUBLIC_HEADERS LUCID_GeoModel
                   PRIVATE_INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS}
                   LINK_LIBRARIES ${EIGEN_LIBRARIES} ${GEOMODELCORE_LIBRARIES} GeoModelUtilities GaudiKernel StoreGateLib SGtests
                   PRIVATE_LINK_LIBRARIES ${Boost_LIBRARIES} ${CORAL_LIBRARIES} AthenaKernel SGTools RDBAccessSvcLib )

atlas_add_component( LUCID_GeoModel
                     src/components/*.cxx
                     INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS}
                     LINK_LIBRARIES ${Boost_LIBRARIES} ${CORAL_LIBRARIES} ${EIGEN_LIBRARIES} ${GEOMODELCORE_LIBRARIES} GeoModelUtilities GaudiKernel AthenaKernel SGTools StoreGateLib SGtests LUCID_GeoModelLib )

