# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( GeoModelXml )

# External dependencies:
find_package( XercesC )
find_package( Eigen ) # Needed by GeoModelKernel
find_package( GeoModel COMPONENTS GeoModelKernel ExpressionEvaluator )
find_package( ZLIB )

# Component(s) in the package:
atlas_add_library( GeoModelXml
   GeoModelXml/*.h src/*.cxx
   PUBLIC_HEADERS GeoModelXml
   INCLUDE_DIRS ${XERCESC_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                ${GEOMODEL_INCLUDE_DIRS}
   PRIVATE_INCLUDE_DIRS ${ZLIB_INCLUDE_DIRS}
   LINK_LIBRARIES ${XERCESC_LIBRARIES} ${EIGEN_LIBRARIES} ${GEOMODEL_LIBRARIES}
   PRIVATE_LINK_LIBRARIES ${ZLIB_LIBRARIES} GaudiKernel GeoModelInterfaces
                          StoreGateLib )

atlas_install_runtime( data/*.dtd )
