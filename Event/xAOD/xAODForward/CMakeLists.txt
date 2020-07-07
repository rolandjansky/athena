# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name.
atlas_subdir( xAODForward )

# Pull in the helper CMake code.
find_package( xAODUtilities )

# Component(s) in the package.
atlas_add_library( xAODForward
   xAODForward/*.h xAODForward/versions/*.h Root/*.cxx
   PUBLIC_HEADERS xAODForward
   LINK_LIBRARIES AthContainers xAODCore xAODTrigL1Calo )

atlas_add_xaod_smart_pointer_dicts(
   INPUT xAODForward/selection.xml
   OUTPUT _selectionFile
   CONTAINERS "xAOD::AFPDataContainer_v1" "xAOD::AFPSiHitContainer_v1"
              "xAOD::AFPSiHitContainer_v2" "xAOD::AFPSiHitsClusterContainer_v1"
              "xAOD::AFPTrackContainer_v1" "xAOD::AFPTrackContainer_v2"
              "xAOD::AFPToFHitContainer_v1" "xAOD::AFPProtonContainer_v1"
              "xAOD::ALFADataContainer_v1" "xAOD::ZdcModuleContainer_v1"
              "xAOD::MBTSModuleContainer_v1"
              "xAOD::ForwardEventInfoContainer_v1" )

atlas_add_dictionary( xAODForwardDict
   xAODForward/xAODForwardDict.h
   ${_selectionFile}
   LINK_LIBRARIES xAODCore xAODForward
   EXTRA_FILES Root/dict/*.cxx )
