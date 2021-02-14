# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( AFP_GeoModel )

# External dependencies:
find_package( CLHEP )
find_package( GeoModel COMPONENTS GeoModelKernel )

# Component(s) in the package:
atlas_add_component( AFP_GeoModel
		     src/*.cxx
                     src/components/*.cxx
		     INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS} ${GEOMODEL_INCLUDE_DIRS}
		     LINK_LIBRARIES ${CLHEP_LIBRARIES} ${GEOMODEL_LIBRARIES} AFP_Geometry AthenaKernel GeoModelUtilities AthenaPoolUtilities GaudiKernel GeoModelInterfaces GeoPrimitives StoreGateLib )

