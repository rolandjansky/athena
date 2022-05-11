# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

find_package( Boost )

# Declare the package name:
atlas_subdir( ActsUtils )

# External dependencies:
# Component(s) in the package:
atlas_add_library( ActsUtilsLib
    ActsUtils/*.h
    INTERFACE
    PUBLIC_HEADERS ActsUtils
    INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
    LINK_LIBRARIES ${Boost_LIBRARIES} AthContainers)
