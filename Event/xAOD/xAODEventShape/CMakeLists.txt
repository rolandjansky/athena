# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name.
atlas_subdir( xAODEventShape )

# Pull in the helper CMake code.
find_package( xAODUtilities )

# Component(s) in the package.
atlas_add_library( xAODEventShape
   xAODEventShape/*.h xAODEventShape/versions/*.h Root/*.cxx
   PUBLIC_HEADERS xAODEventShape
   LINK_LIBRARIES AthContainers xAODCore )

atlas_add_xaod_smart_pointer_dicts(
   INPUT xAODEventShape/selection.xml
   OUTPUT _selectionFile
   OBJECTS "xAOD::EventShape_v1" )

atlas_add_dictionary( xAODEventShapeDict
   xAODEventShape/xAODEventShapeDict.h
   ${_selectionFile}
   LINK_LIBRARIES xAODCore xAODEventShape )
