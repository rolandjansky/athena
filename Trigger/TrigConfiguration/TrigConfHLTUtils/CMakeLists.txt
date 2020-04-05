# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration.

# Declare the package name.
atlas_subdir( TrigConfHLTUtils )

# Add the package's stand alone library
atlas_add_library( TrigConfHLTUtilsLib
  TrigConfHLTUtils/*.h Root/*.cxx
  PUBLIC_HEADERS TrigConfHLTUtils
  LINK_LIBRARIES CxxUtils )

# Executable(s).
atlas_add_executable( trigconf_string2hash
  util/trigconf_string2hash.cxx
  LINK_LIBRARIES TrigConfHLTUtilsLib )
