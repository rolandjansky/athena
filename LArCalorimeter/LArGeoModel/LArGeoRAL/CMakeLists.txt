#Copyright (C) 2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( LArGeoRAL )

# External dependencies:
find_package( Boost COMPONENTS system )
find_package( CLHEP )

# Component(s) in the package:
atlas_add_library( LArGeoRAL
                   src/*.cxx
                   PUBLIC_HEADERS LArGeoRAL
                   PRIVATE_INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                   PRIVATE_DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES LArGeoCode StoreGateLib SGtests
                   PRIVATE_LINK_LIBRARIES ${Boost_LIBRARIES} ${CLHEP_LIBRARIES} GaudiKernel RDBAccessSvcLib GeoModelInterfaces )

