# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrkValEvent )

# External dependencies:
find_package( CLHEP )

# Component(s) in the package:
atlas_add_library( TrkValEvent
                   src/*.cxx
                   PUBLIC_HEADERS TrkValEvent
                   INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS} 
                   DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${CLHEP_LIBRARIES} AtlasHepMCLib TrkEventPrimitives TrkParameters TrkTrack )

