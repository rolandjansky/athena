# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( LArG4MiniFCAL )

# External dependencies:
find_package( CLHEP )
find_package( Geant4 )

# Component(s) in the package:
atlas_add_library( LArG4MiniFCAL
                   src/*.cc
                   src/components/*.cxx
                   OBJECT
                   NO_PUBLIC_HEADERS
                   PRIVATE_INCLUDE_DIRS ${GEANT4_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                   PRIVATE_DEFINITIONS ${CLHEP_DEFINITIONS}
                   PRIVATE_LINK_LIBRARIES ${GEANT4_LIBRARIES} AthenaBaseComps AthenaKernel LArG4Code CaloG4SimLib StoreGateLib ${CLHEP_LIBRARIES} GaudiKernel Identifier GeoModelInterfaces RDBAccessSvcLib )

atlas_install_python_modules( python/*.py )
