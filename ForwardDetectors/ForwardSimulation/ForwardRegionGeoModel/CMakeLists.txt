# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( ForwardRegionGeoModel )

# External dependencies:
find_package( CLHEP )
find_package( GeoModelCore )

# Component(s) in the package:
atlas_add_library( ForwardRegionGeoModelLib
                   src/*.cxx
                   PUBLIC_HEADERS ForwardRegionGeoModel
                   INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                   DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${CLHEP_LIBRARIES} ${GEOMODELCORE_LIBRARIES} AthenaKernel GaudiKernel GeoModelUtilities
                   PRIVATE_LINK_LIBRARIES GeoModelInterfaces PathResolver StoreGateLib )

atlas_add_component( ForwardRegionGeoModel
                     src/components/*.cxx
                     LINK_LIBRARIES ForwardRegionGeoModelLib )

# Install files from the package:
atlas_install_runtime( share/*.csv )

