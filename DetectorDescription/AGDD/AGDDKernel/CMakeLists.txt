# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( AGDDKernel )

# External dependencies:
find_package( GeoModelCore )

# Component(s) in the package:
atlas_add_library( AGDDKernel
                   src/*.cxx
                   PUBLIC_HEADERS AGDDKernel
                   INCLUDE_DIRS ${GEOMODELCORE_INCLUDE_DIRS}
                   LINK_LIBRARIES GeoPrimitives ${GEOMODELCORE_LIBRARIES} )

