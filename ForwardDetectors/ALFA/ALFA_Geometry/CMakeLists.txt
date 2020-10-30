# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( ALFA_Geometry )

# External dependencies:
find_package( CLHEP )

# Component(s) in the package:
atlas_add_library( ALFA_Geometry
                   src/*.cxx
                   PUBLIC_HEADERS ALFA_Geometry
                   INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                   DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${CLHEP_LIBRARIES} AthenaKernel GaudiKernel GeoModelInterfaces RDBAccessSvcLib
                   PRIVATE_LINK_LIBRARIES PathResolver StoreGateLib )

# Install files from the package:
atlas_install_runtime( share/metrology/*.dat )

