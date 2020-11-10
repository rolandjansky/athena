# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrkVertexFitterInterfaces )

# External dependencies:
find_package( CLHEP )

# Component(s) in the package:
atlas_add_library( TrkVertexFitterInterfaces
                   PUBLIC_HEADERS TrkVertexFitterInterfaces
                   INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                   DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${CLHEP_LIBRARIES} GeoPrimitives EventPrimitives xAODTracking GaudiKernel TrkNeutralParameters TrkParameters )
