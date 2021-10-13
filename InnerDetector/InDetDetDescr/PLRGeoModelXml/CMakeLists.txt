# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( PLRGeoModelXml )

# External dependencies:
find_package( GeoModel COMPONENTS GeoModelKernel )

# Component(s) in the package:
atlas_add_library( PLRGeoModelXmlLib
                   src/*.cxx
                   PUBLIC_HEADERS PLRGeoModelXml
                   PRIVATE_INCLUDE_DIRS ${GEOMODEL_INCLUDE_DIRS}
                   LINK_LIBRARIES GaudiKernel GeoModelUtilities GeoModelXml InDetGeoModelUtils PixelReadoutGeometryLib PixelGeoModelXmlLib
                   PRIVATE_LINK_LIBRARIES ${GEOMODEL_LIBRARIES} AthenaBaseComps AthenaPoolUtilities DetDescrConditions GeoModelInterfaces ReadoutGeometryBase InDetReadoutGeometry InDetSimEvent PathResolver RDBAccessSvcLib SGTools StoreGateLib )

atlas_add_component( PLRGeoModelXml
                     src/components/*.cxx
                     LINK_LIBRARIES PLRGeoModelXmlLib )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )

