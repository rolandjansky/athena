# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( HGTD_GeoModelXml )

# External dependencies:
find_package( XercesC )
find_package( GeoModel COMPONENTS GeoModelKernel GeoModelXml ExpressionEvaluator )

# Component(s) in the package:
atlas_add_component( HGTD_GeoModelXml
                     src/*.cxx
                     src/components/*.cxx
                     PRIVATE_INCLUDE_DIRS ${XERCESC_INCLUDE_DIRS} ${GEOMODEL_INCLUDE_DIRS}
                     PRIVATE_LINK_LIBRARIES ${XERCESC_LIBRARIES} ${GEOMODEL_LIBRARIES}
                     GaudiKernel DetDescrConditions HGTD_ReadoutGeometry InDetGeoModelUtils InDetSimEvent )

atlas_install_python_modules( python/*.py )
