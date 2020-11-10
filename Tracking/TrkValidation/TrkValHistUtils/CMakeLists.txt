# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrkValHistUtils )

set( extra_libs )
if( NOT XAOD_ANALYSIS )
    set( extra_libs TrkEventPrimitives )
endif()

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( TrkValHistUtils
                   Root/*.cxx
                   src/*.cxx
                   PUBLIC_HEADERS TrkValHistUtils
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${ROOT_LIBRARIES} xAODBase xAODTracking xAODTruth
                   PRIVATE_LINK_LIBRARIES EventPrimitives ${extra_libs} )

