# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( StripGeoModelXml )

# Component(s) in the package:
atlas_add_library( StripGeoModelXmlLib
                   src/*.cxx
                   PUBLIC_HEADERS StripGeoModelXml
                   LINK_LIBRARIES GaudiKernel GeoModelUtilities GeoModelXml InDetGeoModelUtils SCT_ReadoutGeometry
                   PRIVATE_LINK_LIBRARIES ${GEOMODELCORE_LIBRARIES} AthenaPoolUtilities DetDescrConditions GeoModelInterfaces GeometryDBSvcLib InDetReadoutGeometry InDetSimEvent PathResolver RDBAccessSvcLib SGTools StoreGateLib )

atlas_add_component( StripGeoModelXml
                     src/components/*.cxx
                     LINK_LIBRARIES StripGeoModelXmlLib )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
