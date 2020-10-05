# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name.
atlas_subdir( xAODLuminosity )

# Pull in the helper CMake code.
find_package( xAODUtilities )

# Component(s) in the package:
atlas_add_library( xAODLuminosity
   xAODLuminosty/*.h xAODLuminosity/versions/*.h Root/*.cxx
   PUBLIC_HEADERS xAODLuminosity
   LINK_LIBRARIES AthContainers xAODCore )

atlas_add_xaod_smart_pointer_dicts(
   INPUT xAODLuminosity/selection.xml
   OUTPUT _selectionFile
   CONTAINERS "xAOD::LumiBlockRangeContainer_v1"
              "xAOD::BCMRawDataContainer_v1" )

atlas_add_dictionary( xAODLuminosityDict
   xAODLuminosity/xAODLuminosityDict.h
   ${_selectionFile}
   LINK_LIBRARIES xAODCore xAODLuminosity
   EXTRA_FILES Root/dict/*.cxx )

# Generate CLIDs from the library:
atlas_generate_cliddb( xAODLuminosity )
