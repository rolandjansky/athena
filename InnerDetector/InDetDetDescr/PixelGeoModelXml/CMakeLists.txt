# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( PixelGeoModelXml )

# External dependencies:
find_package( XercesC )
find_package( GeoModel COMPONENTS GeoModelKernel GeoModelXml )

# Component(s) in the package:
atlas_add_library( PixelGeoModelXmlLib
                   src/*.cxx
                   PUBLIC_HEADERS PixelGeoModelXml
                   PRIVATE_INCLUDE_DIRS ${XERCESC_INCLUDE_DIRS} ${GEOMODEL_INCLUDE_DIRS}
                   LINK_LIBRARIES GaudiKernel GeoModelUtilities InDetGeoModelUtils PixelReadoutGeometryLib CxxUtils
                   PRIVATE_LINK_LIBRARIES ${XERCESC_LIBRARIES} ${GEOMODEL_LIBRARIES} AthenaBaseComps AthenaPoolUtilities DetDescrConditions GeoModelInterfaces GeometryDBSvcLib ReadoutGeometryBase InDetReadoutGeometry InDetSimEvent PathResolver RDBAccessSvcLib SGTools StoreGateLib )

atlas_add_component( PixelGeoModelXml
                     src/components/*.cxx
                     LINK_LIBRARIES PixelGeoModelXmlLib )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )

# Test(s) in the package:
atlas_add_test( ITkPixelGMConfig_test
                SCRIPT test/ITkPixelGMConfig_test.py
                POST_EXEC_SCRIPT nopost.sh )
