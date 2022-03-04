# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( InDetGeoModelUtils )

# External dependencies:

find_package( GeoModel COMPONENTS GeoModelKernel GeoModelXml ExpressionEvaluator )
find_package( XercesC  )

# Component(s) in the package:
atlas_add_library( InDetGeoModelUtils
                   src/*.cxx
                   PUBLIC_HEADERS InDetGeoModelUtils
                   INCLUDE_DIRS ${XERCESC_INCLUDE_DIRS} ${GEOMODEL_INCLUDE_DIRS}
                   LINK_LIBRARIES ${GEOMODEL_LIBRARIES} ${XERCESC_LIBRARIES}
                   AthenaKernel AthenaBaseComps CxxUtils GaudiKernel
                   GeoModelInterfaces GeoModelUtilities GeoPrimitives PathResolver RDBAccessSvcLib
                   PRIVATE_LINK_LIBRARIES GeometryDBSvcLib StoreGateLib )
