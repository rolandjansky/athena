# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name.
atlas_subdir( xAODCutFlow )

# External dependencies.
find_package( ROOT COMPONENTS Core Hist )
find_package( xAODUtilities )

# Component(s) in the package.
atlas_add_library( xAODCutFlow
   xAODCutFlow/*.h Root/*.cxx
   PUBLIC_HEADERS xAODCutFlow
   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES AthContainers AthLinks xAODCore
   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} )

atlas_add_xaod_smart_pointer_dicts(
   INPUT xAODCutFlow/selection.xml
   OUTPUT _selectionFile
   CONTAINERS "xAOD::CutBookkeeperContainer_v1" )

atlas_add_dictionary( xAODCutFlowDict
   xAODCutFlow/xAODCutFlowDict.h
   ${_selectionFile}
   LINK_LIBRARIES xAODCore xAODCutFlow
   EXTRA_FILES Root/dict/*.cxx )
