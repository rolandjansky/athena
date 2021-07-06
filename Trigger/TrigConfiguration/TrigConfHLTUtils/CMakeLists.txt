# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration.

# Declare the package name.
atlas_subdir( TrigConfHLTUtils )

find_package(TBB)

# Add the package's stand alone library
atlas_add_library( TrigConfHLTUtilsLib
  TrigConfHLTUtils/*.h Root/*.cxx
  PUBLIC_HEADERS TrigConfHLTUtils
  INCLUDE_DIRS ${TBB_INCLUDE_DIRS}
  LINK_LIBRARIES CxxUtils ${TBB_LIBRARIES} )

# Executable(s).
atlas_add_executable( trigconf_string2hash
  util/trigconf_string2hash.cxx
  LINK_LIBRARIES TrigConfHLTUtilsLib )
