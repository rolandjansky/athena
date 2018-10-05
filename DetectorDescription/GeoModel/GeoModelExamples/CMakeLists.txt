################################################################################
# Package: GeoModelExamples
################################################################################

# Declare the package name:
atlas_subdir( GeoModelExamples )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaKernel
                          DetectorDescription/GeoModel/GeoModelUtilities
                          PRIVATE
                          Control/StoreGate
                          DetectorDescription/GeoModel/GeoModelInterfaces
                          DetectorDescription/Identifier
                          GaudiKernel )

# External dependencies:
find_package( Eigen )
find_package( GeoModel )

# Component(s) in the package:
atlas_add_library( GeoModelExamplesLib
                   src/*.cxx
                   PUBLIC_HEADERS GeoModelExamples
                   PRIVATE_INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS} ${GEOMODEL_INCLUDE_DIRS}
                   LINK_LIBRARIES GeoModelUtilities StoreGateLib
                   PRIVATE_LINK_LIBRARIES ${EIGEN_LIBRARIES} ${GEOMODEL_LIBRARIES} Identifier GaudiKernel )

atlas_add_component( GeoModelExamples
                     src/components/*.cxx
                     INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS} ${GEOMODEL_INCLUDE_DIRS}
                     LINK_LIBRARIES ${EIGEN_LIBRARIES} ${GEOMODEL_LIBRARIES} GeoModelUtilities StoreGateLib Identifier GaudiKernel GeoModelExamplesLib )

