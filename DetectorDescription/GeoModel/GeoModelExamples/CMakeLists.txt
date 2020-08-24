################################################################################
# Package: GeoModelExamples
################################################################################

# Declare the package name:
atlas_subdir( GeoModelExamples )

# External dependencies:
find_package( Eigen )
find_package( GeoModelCore )

# Component(s) in the package:
if(BUILDVP1LIGHT)
    atlas_add_library( GeoModelExamplesLib
                       src/*.cxx
                       PUBLIC_HEADERS GeoModelExamples
                       PRIVATE_INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                       PRIVATE_DEFINITIONS ${CLHEP_DEFINITIONS}
                       LINK_LIBRARIES GeoModelUtilities
                       PRIVATE_LINK_LIBRARIES ${CLHEP_LIBRARIES} )

    atlas_add_component( GeoModelExamples
                         src/components/*.cxx
                         INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                         LINK_LIBRARIES ${CLHEP_LIBRARIES} GeoModelUtilities GeoModelExamplesLib )
else()
atlas_add_library( GeoModelExamplesLib
                   src/*.cxx
                   PUBLIC_HEADERS GeoModelExamples
                   PRIVATE_INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS} ${GEOMODELCORE_INCLUDE_DIRS}
                   LINK_LIBRARIES GeoModelUtilities StoreGateLib
                   PRIVATE_LINK_LIBRARIES ${EIGEN_LIBRARIES} ${GEOMODELCORE_LIBRARIES} Identifier GaudiKernel )

atlas_add_component( GeoModelExamples
                     src/components/*.cxx
                     INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS} ${GEOMODELCORE_INCLUDE_DIRS}
                     LINK_LIBRARIES ${EIGEN_LIBRARIES} ${GEOMODELCORE_LIBRARIES} GeoModelUtilities StoreGateLib Identifier GaudiKernel GeoModelExamplesLib )
endif()


