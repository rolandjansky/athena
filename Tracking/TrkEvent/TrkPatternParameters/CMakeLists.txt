# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrkPatternParameters )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )

# Component(s) in the package:
atlas_add_library( TrkPatternParameters
                   src/*.cxx
                   PUBLIC_HEADERS TrkPatternParameters
                   PRIVATE_INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
                   LINK_LIBRARIES TrkEventPrimitives TrkParametersBase
                   PRIVATE_LINK_LIBRARIES ${Boost_LIBRARIES} GaudiKernel TrkSurfaces )

