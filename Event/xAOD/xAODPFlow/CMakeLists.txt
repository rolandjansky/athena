# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name.
atlas_subdir( xAODPFlow )

# External dependencies.
find_package( ROOT COMPONENTS Core Physics GenVector )
find_package( xAODUtilities )

# Component(s) in the package:
atlas_add_library( xAODPFlow
   xAODPFlow/*.h xAODPFlow/versions/*.h xAODPFlow/versions/*.icc Root/*.cxx
   PUBLIC_HEADERS xAODPFlow
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} AthContainers AthLinks xAODBase
   xAODCaloEvent xAODCore xAODTracking )

atlas_add_xaod_smart_pointer_dicts(
   INPUT xAODPFlow/selection.xml
   OUTPUT _selectionFile
   CONTAINERS "xAOD::PFOContainer_v1" "xAOD::TrackCaloClusterContainer_v1"  "xAOD::FlowElementContainer_v1")

atlas_add_dictionary( xAODPFlowDict
   xAODPFlow/xAODPFlowDict.h
   ${_selectionFile}
   LINK_LIBRARIES AthLinks xAODCore xAODPFlow
   EXTRA_FILES Root/dict/*.cxx )
