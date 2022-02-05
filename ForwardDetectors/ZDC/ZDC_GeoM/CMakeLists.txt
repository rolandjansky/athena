# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( ZDC_GeoM )

# External dependencies:
find_package( GeoModel COMPONENTS GeoModelKernel )

# Component(s) in the package:
atlas_add_component( ZDC_GeoM
		     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES ${GEOMODEL_LIBRARIES} AthenaKernel GeoModelUtilities GaudiKernel GeoModelInterfaces StoreGateLib AthenaBaseComps )
