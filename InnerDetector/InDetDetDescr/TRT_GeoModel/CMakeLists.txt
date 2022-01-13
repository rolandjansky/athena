# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TRT_GeoModel )

# External dependencies:
find_package( GeoModel COMPONENTS GeoModelKernel GeoModelDBManager GeoModelRead)

# Component(s) in the package:
atlas_add_component( TRT_GeoModel
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES ${GEOMODEL_LIBRARIES} AthenaKernel GeoModelInterfaces GeoModelUtilities GeoPrimitives GaudiKernel TRT_ConditionsServicesLib InDetGeoModelUtils ReadoutGeometryBase InDetReadoutGeometry TRT_ReadoutGeometry CxxUtils SGTools StoreGateLib AthenaPoolUtilities DetDescrConditions IdDictDetDescr TRT_ConditionsData InDetIdentifier GeometryDBSvcLib RDBAccessSvcLib )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_scripts( test/*.py )

# Test(s) in the package:
atlas_add_test( TRT_GMConfig_test
                SCRIPT test/TRT_GMConfig_test.py
                POST_EXEC_SCRIPT nopost.sh
                PROPERTIES TIMEOUT 600 )
