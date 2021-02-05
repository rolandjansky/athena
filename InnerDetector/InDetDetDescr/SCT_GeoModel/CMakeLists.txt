# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( SCT_GeoModel )

# External dependencies:
find_package( GeoModel COMPONENTS GeoModelKernel )

# Component(s) in the package:
atlas_add_component( SCT_GeoModel
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES ${GEOMODEL_LIBRARIES} AthenaKernel CxxUtils GeoModelInterfaces GeoModelUtilities GeoPrimitives GaudiKernel InDetGeoModelUtils ReadoutGeometryBase InDetReadoutGeometry SCT_ReadoutGeometry SGTools StoreGateLib AthenaPoolUtilities DetDescrConditions Identifier InDetIdentifier GeometryDBSvcLib RDBAccessSvcLib )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_scripts( test/*.py )

# Test(s) in the package:
atlas_add_test( SCT_GMConfig_test
                SCRIPT test/SCT_GMConfig_test.py
                PROPERTIES TIMEOUT 300 )
