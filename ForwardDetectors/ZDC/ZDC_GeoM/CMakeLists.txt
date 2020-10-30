# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( ZDC_GeoM )

# External dependencies:
find_package( GeoModelCore )

# Component(s) in the package:
atlas_add_library( ZDC_GeoMLib
                   src/*.cxx
                   PUBLIC_HEADERS ZDC_GeoM
                   LINK_LIBRARIES ${GEOMODELCORE_LIBRARIES} AthenaKernel GeoModelUtilities
                   PRIVATE_LINK_LIBRARIES GaudiKernel GeoModelInterfaces StoreGateLib )

atlas_add_component( ZDC_GeoM
                     src/components/*.cxx
                     LINK_LIBRARIES ZDC_GeoMLib )
