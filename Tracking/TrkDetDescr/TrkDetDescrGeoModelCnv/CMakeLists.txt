# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrkDetDescrGeoModelCnv )

# External dependencies:
find_package( GeoModelCore )

# Component(s) in the package:
atlas_add_library( TrkDetDescrGeoModelCnv
                   src/*.cxx
                   PUBLIC_HEADERS TrkDetDescrGeoModelCnv
                   PRIVATE_INCLUDE_DIRS ${GEOMODELCORE_INCLUDE_DIRS}
                   LINK_LIBRARIES GeoPrimitives TrkGeometry
                   PRIVATE_LINK_LIBRARIES ${GEOMODELCORE_LIBRARIES} GaudiKernel TrkVolumes )
