# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TilePulseSimulator )

# External dependencies:
find_package( ROOT COMPONENTS Hist Core MathCore )

# Component(s) in the package:
atlas_add_library( TilePulseSimulator
                   src/*.cxx
                   PUBLIC_HEADERS TilePulseSimulator
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaBaseComps
                   PRIVATE_LINK_LIBRARIES GaudiKernel )
