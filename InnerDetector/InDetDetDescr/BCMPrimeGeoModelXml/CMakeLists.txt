# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( BCMPrimeGeoModelXml )

# External dependencies:
find_package( GeoModel COMPONENTS GeoModelKernel )

# Component(s) in the package:
atlas_add_library( BCMPrimeGeoModelXmlLib
                   src/*.cxx
                   PUBLIC_HEADERS BCMPrimeGeoModelXml
                   PRIVATE_INCLUDE_DIRS ${GEOMODEL_INCLUDE_DIRS}
                   LINK_LIBRARIES GaudiKernel GeoModelUtilities GeoModelXml InDetGeoModelUtils BCMPrimeReadoutGeometry
                   PRIVATE_LINK_LIBRARIES ${GEOMODEL_LIBRARIES} AthenaPoolUtilities DetDescrConditions GeoModelInterfaces GeometryDBSvcLib InDetSimEvent PathResolver RDBAccessSvcLib SGTools StoreGateLib )

atlas_add_component( BCMPrimeGeoModelXml
                     src/components/*.cxx
                     LINK_LIBRARIES BCMPrimeGeoModelXmlLib )
                     
# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
