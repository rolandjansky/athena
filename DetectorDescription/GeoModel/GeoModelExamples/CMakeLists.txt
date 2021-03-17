# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( GeoModelExamples )

# External dependencies:
find_package( Eigen ) # Needed by GeoModelKernel
find_package( GeoModel COMPONENTS GeoGenericFunctions GeoModelKernel )

# The list of linked libraries depends on what project is being built.
if( NOT BUILDVP1LIGHT )
   set( extra_libs PRIVATE_LINK_LIBRARIES GaudiKernel Identifier StoreGateLib
      GeoModelInterfaces GeoModelUtilities )
endif()

# Component(s) in the package:
atlas_add_library( GeoModelExamplesLib
   GeoModelExamples/*.h src/*.cxx
   PUBLIC_HEADERS GeoModelExamples
   INCLUDE_DIRS ${GEOMODEL_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
   LINK_LIBRARIES ${GEOMODEL_LIBRARIES} ${EIGEN_LIBRARIES}
   ${extra_libs} )

if( NOT BUILDVP1LIGHT )
   atlas_add_component( GeoModelExamples
      src/components/*.cxx
      INCLUDE_DIRS ${GEOMODEL_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
      LINK_LIBRARIES ${GEOMODEL_LIBRARIES} GeoModelUtilities
      GeoModelExamplesLib )
endif()
