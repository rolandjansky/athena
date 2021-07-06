# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( AGDDKernel )

# External dependencies:
find_package( GeoModel COMPONENTS GeoModelKernel )

# Component(s) in the package:
atlas_add_library( AGDDKernel
   AGDDKernel/*.h src/*.cxx
   PUBLIC_HEADERS AGDDKernel
   INCLUDE_DIRS ${GEOMODEL_INCLUDE_DIRS}
   LINK_LIBRARIES GeoPrimitives ${GEOMODEL_LIBRARIES} )
