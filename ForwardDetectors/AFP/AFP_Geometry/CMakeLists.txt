# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( AFP_Geometry )

# External dependencies:
find_package( CLHEP )

# Component(s) in the package:
atlas_add_library( AFP_Geometry
                   src/*.cxx
                   PUBLIC_HEADERS AFP_Geometry
                   INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                   DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${CLHEP_LIBRARIES} AthenaKernel
                   PRIVATE_LINK_LIBRARIES GaudiKernel PathResolver StoreGateLib )
