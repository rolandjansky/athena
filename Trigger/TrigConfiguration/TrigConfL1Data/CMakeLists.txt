# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigConfL1Data )

# External dependencies:
find_package( Boost )

# Component(s) in the package:
atlas_add_library( TrigConfL1Data
  TrigConfL1Data/*.h Root/*.cxx
  PUBLIC_HEADERS TrigConfL1Data
  INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
  LINK_LIBRARIES ${Boost_LIBRARIES} )
