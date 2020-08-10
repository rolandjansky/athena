# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name.
atlas_subdir( ActsInterop )

# External dependencies:
find_package( Acts COMPONENTS Core )
find_package( Boost )

# Component(s) in the package.
atlas_add_library( ActsInteropLib
   ActsInterop/*.h src/*.cxx
   PUBLIC_HEADERS ActsInterop
   INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
   LINK_LIBRARIES ${Boost_LIBRARIES} ActsCore GaudiKernel
   PRIVATE_LINK_LIBRARIES InDetIdentifier InDetReadoutGeometry )
