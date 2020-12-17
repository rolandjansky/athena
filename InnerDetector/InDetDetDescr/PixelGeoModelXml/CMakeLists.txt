# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( PixelGeoModelXml )

# External dependencies:
find_package( GeoModelCore )

# Component(s) in the package:
atlas_add_library( PixelGeoModelXmlLib
                   src/*.cxx
                   PUBLIC_HEADERS PixelGeoModelXml
                   PRIVATE_INCLUDE_DIRS ${GEOMODELCORE_INCLUDE_DIRS}
                   LINK_LIBRARIES GaudiKernel GeoModelUtilities GeoModelXml InDetGeoModelUtils PixelReadoutGeometry
                   PRIVATE_LINK_LIBRARIES ${GEOMODELCORE_LIBRARIES} AthenaPoolUtilities DetDescrConditions GeoModelInterfaces GeometryDBSvcLib ReadoutGeometryBase InDetReadoutGeometry InDetSimEvent PathResolver RDBAccessSvcLib SGTools StoreGateLib )

atlas_add_component( PixelGeoModelXml
                     src/components/*.cxx
		             LINK_LIBRARIES PixelGeoModelXmlLib )
                     
# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
