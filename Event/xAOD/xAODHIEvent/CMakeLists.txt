# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name.
atlas_subdir( xAODHIEvent )

# Pull in the helper CMake code.
find_package( xAODUtilities )

# Component(s) in the package.
atlas_add_library( xAODHIEvent
   xAODHIEvent/versions/*.h xAODHIEvent/*.h Root/*.cxx
   PUBLIC_HEADERS xAODHIEvent
   LINK_LIBRARIES AthContainers AthLinks xAODCore )

atlas_add_xaod_smart_pointer_dicts(
   INPUT xAODHIEvent/selection.xml
   OUTPUT _selectionFile
   CONTAINERS "xAOD::HIEventShapeContainer_v1"
              "xAOD::HIEventShapeContainer_v2" )

atlas_add_dictionary( xAODHIEventDict
   xAODHIEvent/xAODHIEventDict.h
   ${_selectionFile}
   LINK_LIBRARIES xAODCore xAODHIEvent
   EXTRA_FILES Root/dict/*.cxx )
