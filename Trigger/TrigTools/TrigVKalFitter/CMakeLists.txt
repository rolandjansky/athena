# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigVKalFitter )

# External dependencies:
find_package( CLHEP )

# Component(s) in the package:
atlas_add_library( TrigVKalFitterLib
                   src/*.cxx
                   PUBLIC_HEADERS TrigVKalFitter
                   INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                   DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${CLHEP_LIBRARIES} AthenaBaseComps EventPrimitives GaudiKernel GeoPrimitives MagFieldInterfaces TrkVKalVrtCore
                   PRIVATE_LINK_LIBRARIES TrigInDetEvent )

atlas_add_component( TrigVKalFitter
                     src/components/*.cxx
                     LINK_LIBRARIES TrigVKalFitterLib )
