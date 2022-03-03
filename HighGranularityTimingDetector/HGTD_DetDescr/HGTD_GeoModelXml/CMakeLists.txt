# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( HGTD_GeoModelXml )

# External dependencies:
find_package( XercesC )
find_package( GeoModel COMPONENTS GeoModelKernel GeoModelXml )
find_package( Boost COMPONENTS filesystem thread system )
find_package( CLHEP )
find_package( Eigen )
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )

# Component(s) in the package:
atlas_add_library( HGTD_GeoModelXmlLib
                   src/*.cxx
                   PUBLIC_HEADERS HGTD_GeoModelXml
                   INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS} ${XERCESC_INCLUDE_DIRS} ${GEOMODEL_INCLUDE_DIRS}
                   DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${Boost_LIBRARIES} ${CORAL_LIBRARIES} ${CLHEP_LIBRARIES} ${XERCESC_LIBRARIES} ${GEOMODEL_LIBRARIES} AthenaKernel PathResolver GeoModelUtilities GaudiKernel InDetGeoModelUtils InDetSimEvent GeometryDBSvcLib HGTD_GeoModelLib HGTD_Identifier HGTD_ReadoutGeometry ReadoutGeometryBase StoreGateLib SGtests AthenaBaseComps
                   PRIVATE_LINK_LIBRARIES SGTools AthenaPoolUtilities DetDescrConditions Identifier )

atlas_add_component( HGTD_GeoModelXml
                     src/components/*.cxx
                     LINK_LIBRARIES GaudiKernel HGTD_GeoModelXmlLib )

atlas_install_python_modules( python/*.py )

