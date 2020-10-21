# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( InDetGeoModelUtils )

# External dependencies:
find_package( GeoModelCore )

# Component(s) in the package:
atlas_add_library( InDetGeoModelUtils
                   src/*.cxx
                   PUBLIC_HEADERS InDetGeoModelUtils
                   LINK_LIBRARIES ${GEOMODELCORE_LIBRARIES} AthenaKernel CxxUtils GaudiKernel RDBAccessSvcLib GeoPrimitives
                   PRIVATE_LINK_LIBRARIES GeoModelInterfaces GeoModelUtilities GeometryDBSvcLib StoreGateLib )
