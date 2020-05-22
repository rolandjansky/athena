# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name.
atlas_subdir( xAODTrigRinger )

# Pull in the helper CMake code.
find_package( xAODUtilities )

# Component(s) in the package.
atlas_add_library( xAODTrigRinger
   xAODTrigRinger/*.h xAODTrigRinger/versions/*.h Root/*.cxx
   PUBLIC_HEADERS xAODTrigRinger
   LINK_LIBRARIES AthContainers AthLinks xAODCore xAODTrigCalo )

atlas_add_xaod_smart_pointer_dicts(
   INPUT xAODTrigRinger/selection.xml
   OUTPUT _selectionFile
   CONTAINERS "TrigRingerRingsContainer_v1" "TrigRingerRingsContainer_v2"
              "TrigRNNOutputContainer_v1" "TrigRNNOutputContainer_v2" )

atlas_add_dictionary( xAODTrigRingerDict
   xAODTrigRinger/xAODTrigRingerDict.h
   ${_selectionFile}
   LINK_LIBRARIES xAODCore xAODTrigRinger
   EXTRA_FILES Root/dict/*.cxx )
