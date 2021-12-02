# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( HGTD_GeoModel )

# External dependencies:
find_package( CLHEP )
find_package( GeoModel COMPONENTS GeoModelKernel )

# Component(s) in the package:
atlas_add_library( HGTD_GeoModelLib
   HGTD_GeoModel/*.h src/*.cxx
   PUBLIC_HEADERS HGTD_GeoModel
   INCLUDE_DIRS ${GEOMODEL_INCLUDE_DIRS}
   PRIVATE_INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
   PRIVATE_DEFINITIONS ${CLHEP_DEFINITIONS}
   LINK_LIBRARIES ${GEOMODEL_LIBRARIES} GaudiKernel StoreGateLib
   InDetGeoModelUtils HGTD_ReadoutGeometry GeoModelUtilities GeoModelInterfaces
   RDBAccessSvcLib InDetGeoModelUtils
   PRIVATE_LINK_LIBRARIES ${CLHEP_LIBRARIES} AthenaBaseComps AthenaKernel
   SGTools AthenaPoolUtilities DetDescrConditions HGTD_Identifier
   ReadoutGeometryBase )

atlas_add_component( HGTD_GeoModel
   src/components/*.cxx
   LINK_LIBRARIES HGTD_GeoModelLib )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
