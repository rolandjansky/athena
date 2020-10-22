# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrkDetElementBase )

# External dependencies:
find_package( GeoModelCore )

# Component(s) in the package:
atlas_add_library( TrkDetElementBase
                   src/*.cxx
                   PUBLIC_HEADERS TrkDetElementBase
                   INCLUDE_DIRS ${GEOMODELCORE_INCLUDE_DIRS}
                   LINK_LIBRARIES ${GEOMODELCORE_LIBRARIES} GeoPrimitives Identifier EventPrimitives )
