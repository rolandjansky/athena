# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( HGTD_GeoModel )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( CLHEP )
find_package( Eigen )
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )

# Component(s) in the package:
atlas_add_library( HGTD_GeoModelLib
                   src/*.cxx
                   PUBLIC_HEADERS HGTD_GeoModel
                   INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                   DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${Boost_LIBRARIES} ${CORAL_LIBRARIES} ${CLHEP_LIBRARIES} AthenaKernel GeoModelUtilities GaudiKernel InDetGeoModelUtils GeometryDBSvcLib HGTD_Identifier HGTD_ReadoutGeometry ReadoutGeometryBase StoreGateLib SGtests AthenaBaseComps
                   PRIVATE_LINK_LIBRARIES SGTools AthenaPoolUtilities DetDescrConditions Identifier )

atlas_add_component( HGTD_GeoModel
                     src/components/*.cxx
                     LINK_LIBRARIES GaudiKernel HGTD_GeoModelLib )

atlas_install_python_modules( python/*.py )
