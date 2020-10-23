# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( PixelGeoModel )

# External dependencies:
find_package( GeoModelCore )

# Component(s) in the package:
atlas_add_library( PixelGeoModelLib
   PixelGeoModel/*.h src/*.cxx
   PUBLIC_HEADERS PixelGeoModel
   LINK_LIBRARIES AthenaBaseComps GaudiKernel GeoModelUtilities GeometryDBSvcLib InDetGeoModelUtils
   PRIVATE_LINK_LIBRARIES ${GEOMODELCORE_LIBRARIES} AthenaKernel CxxUtils DetDescrConditions GeoModelInterfaces GeoPrimitives Identifier InDetIdentifier InDetReadoutGeometry PixelReadoutGeometry RDBAccessSvcLib SGTools StoreGateLib
   PRIVATE_DEFINITIONS "-DGEOTORUS=1" )

atlas_add_component( PixelGeoModel
   src/components/*.cxx
   LINK_LIBRARIES PixelGeoModelLib )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_scripts( test/*.py )

# Test(s) in the package:
atlas_add_test( PixelGMConfig_test
                SCRIPT test/PixelGMConfig_test.py
                PROPERTIES TIMEOUT 300 )
