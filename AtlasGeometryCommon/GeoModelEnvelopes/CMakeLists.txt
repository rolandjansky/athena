# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( GeoModelEnvelopes )

# External dependencies:
find_package( GeoModelCore )

# Component(s) in the package:
atlas_add_component( GeoModelEnvelopes
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES ${GEOMODELCORE_LIBRARIES} AthenaKernel GaudiKernel GeoModelInterfaces GeoModelUtilities SGTools StoreGateLib )
