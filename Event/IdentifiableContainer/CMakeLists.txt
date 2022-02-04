# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

find_package( Boost )

# Declare the package name:
atlas_subdir( IdentifiableContainer )

# External dependencies:
# Component(s) in the package:
atlas_add_library( IdentifiableContainerLib
    IdentifiableContainer/*.h
    INTERFACE
    PUBLIC_HEADERS IdentifiableContainer
    INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
    LINK_LIBRARIES ${Boost_LIBRARIES} AthContainers)
