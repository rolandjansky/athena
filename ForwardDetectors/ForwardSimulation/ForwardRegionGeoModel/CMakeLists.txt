# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( ForwardRegionGeoModel )

# External dependencies:
find_package( CLHEP )
find_package( GeoModel COMPONENTS GeoModelKernel )

# Component(s) in the package:
atlas_add_library( ForwardRegionGeoModelLib
		   ForwardRegionGeoModel/*.h
		   INTERFACE
                   PUBLIC_HEADERS ForwardRegionGeoModel
                   INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                   LINK_LIBRARIES ${CLHEP_LIBRARIES} GaudiKernel )

atlas_add_component( ForwardRegionGeoModel
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES ${GEOMODEL_LIBRARIES} ForwardRegionGeoModelLib AthenaKernel  GeoModelUtilities GeoModelInterfaces PathResolver StoreGateLib )

# Install files from the package:
atlas_install_runtime( share/*.csv )

