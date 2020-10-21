# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TRT_GeoModel )

# External dependencies:
find_package( GeoModelCore )

# Component(s) in the package:
atlas_add_component( TRT_GeoModel
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES ${GEOMODELCORE_LIBRARIES} AthenaKernel GeoModelInterfaces GeoModelUtilities GeoPrimitives GaudiKernel TRT_ConditionsServicesLib InDetGeoModelUtils InDetReadoutGeometry TRT_ReadoutGeometry CxxUtils SGTools StoreGateLib AthenaPoolUtilities DetDescrConditions IdDictDetDescr TRT_ConditionsData InDetIdentifier GeometryDBSvcLib RDBAccessSvcLib )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_scripts( test/*.py )

# Test(s) in the package:
atlas_add_test( TRT_GMConfig_test
                SCRIPT test/TRT_GMConfig_test.py
                PROPERTIES TIMEOUT 600 )
