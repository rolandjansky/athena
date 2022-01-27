# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration.

# Declare the package name:
atlas_subdir( TrigConfHLTUtils )

# External dependencies:
find_package(TBB)

# Component(s) in the package:
atlas_add_library( TrigConfHLTUtilsLib
  TrigConfHLTUtils/*.h Root/*.cxx
  PUBLIC_HEADERS TrigConfHLTUtils
  INCLUDE_DIRS ${TBB_INCLUDE_DIRS}
  LINK_LIBRARIES CxxUtils ${TBB_LIBRARIES} )

atlas_add_executable( trigconf_string2hash
  util/trigconf_string2hash.cxx
  LINK_LIBRARIES TrigConfHLTUtilsLib )

atlas_install_python_modules( python/*.py )
