################################################################################
# Package: GeoModelUtilities
################################################################################

# Declare the package name:
atlas_subdir( GeoModelUtilities )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Database/RDBAccessSvc
                          DetectorDescription/GeoModel/GeoModelInterfaces
                          PRIVATE
                          GaudiKernel )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( Eigen )
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )
find_package( GeoModel )

# Component(s) in the package:
atlas_add_library( GeoModelUtilities
                   src/*.cxx
                   PUBLIC_HEADERS GeoModelUtilities
                   INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS} ${GEOMODEL_INCLUDE_DIRS}
                   LINK_LIBRARIES ${Boost_LIBRARIES} ${CORAL_LIBRARIES} ${EIGEN_LIBRARIES} ${GEOMODEL_LIBRARIES} AthenaBaseComps SGTools 
                   PRIVATE_LINK_LIBRARIES GaudiKernel )

