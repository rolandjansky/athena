# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( AGDDControl )

# External dependencies:
find_package( CLHEP )
find_package( XercesC )
find_package( GeoModel COMPONENTS GeoModelKernel )
find_package( ROOT COMPONENTS Core )

# Component(s) in the package:
atlas_add_library( AGDDControl
   AGDDControl/*.h src/*.cxx
   PUBLIC_HEADERS AGDDControl
   INCLUDE_DIRS ${XERCESC_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                ${GEOMODEL_INCLUDE_DIRS}
   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   DEFINITIONS ${CLHEP_DEFINITIONS}
   LINK_LIBRARIES ${XERCESC_LIBRARIES} ${CLHEP_LIBRARIES} ${GEOMODEL_LIBRARIES}
                  AthenaBaseComps AGDDKernel AGDDModel CxxUtils GaudiKernel
   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaKernel
                          GeoModelInterfaces GeoModelUtilities PathResolver
                          StoreGateLib )
