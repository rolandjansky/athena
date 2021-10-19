# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( BCMPrimeGeoModelXml )

# External dependencies:
find_package( GeoModel COMPONENTS GeoModelKernel )

# Component(s) in the package:
atlas_add_component( BCMPrimeGeoModelXml
                     src/*.cxx
                     src/components/*.cxx
                     PRIVATE_INCLUDE_DIRS ${GEOMODEL_INCLUDE_DIRS}
                     LINK_LIBRARIES GaudiKernel GeoModelUtilities GeoModelXml InDetGeoModelUtils BCMPrimeReadoutGeometry
                     PRIVATE_LINK_LIBRARIES ${GEOMODEL_LIBRARIES} AthenaPoolUtilities DetDescrConditions GeoModelInterfaces InDetSimEvent PathResolver RDBAccessSvcLib SGTools StoreGateLib )
                     
# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
