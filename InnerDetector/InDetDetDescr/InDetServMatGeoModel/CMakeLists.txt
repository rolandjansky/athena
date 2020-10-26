# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( InDetServMatGeoModel )

# External dependencies:
find_package( GeoModelCore )

# Component(s) in the package:
atlas_add_component( InDetServMatGeoModel
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES ${GEOMODELCORE_LIBRARIES} AthenaBaseComps AthenaKernel CxxUtils GaudiKernel GeoModelInterfaces GeoModelUtilities GeoPrimitives GeometryDBSvcLib InDetGeoModelUtils RDBAccessSvcLib SGTools StoreGateLib )
