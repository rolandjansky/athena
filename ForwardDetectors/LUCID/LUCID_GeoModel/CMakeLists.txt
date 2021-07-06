# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( LUCID_GeoModel )

# External dependencies:
find_package( GeoModel COMPONENTS GeoModelKernel )

# Component(s) in the package:
atlas_add_library( LUCID_GeoModelLib
                   src/*.cxx
                   PUBLIC_HEADERS LUCID_GeoModel
                   INCLUDE_DIRS ${GEOMODEL_INCLUDE_DIRS}
                   LINK_LIBRARIES ${GEOMODEL_LIBRARIES} AthenaKernel GeoModelUtilities
                   PRIVATE_LINK_LIBRARIES GaudiKernel GeoModelInterfaces RDBAccessSvcLib SGTools StoreGateLib )

atlas_add_component( LUCID_GeoModel
                     src/components/*.cxx
                     LINK_LIBRARIES LUCID_GeoModelLib )
