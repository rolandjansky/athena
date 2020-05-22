# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name.
atlas_subdir( xAODTrigBphys )

# Pull in the helper CMake code.
find_package( xAODUtilities )

# Component(s) in the package.
atlas_add_library( xAODTrigBphys
   xAODTrigBphys/*.h xAODTrigBphys/versions/*.h Root/*.cxx
   PUBLIC_HEADERS xAODTrigBphys
   LINK_LIBRARIES AthContainers AthLinks xAODCore xAODTracking )

atlas_add_xaod_smart_pointer_dicts(
   INPUT xAODTrigBphys/selection.xml
   OUTPUT _selectionFile
   CONTAINERS "xAOD::TrigBphysContainer_v1" )

atlas_add_dictionary( xAODTrigBphysDict
   xAODTrigBphys/xAODTrigBphysDict.h
   ${_selectionFile}
   LINK_LIBRARIES xAODTrigBphys
   EXTRA_FILES Root/dict/*.cxx )
