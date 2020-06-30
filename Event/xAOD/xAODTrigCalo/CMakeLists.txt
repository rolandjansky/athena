# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name.
atlas_subdir( xAODTrigCalo )

# Pull in the helper CMake code.
find_package( xAODUtilities )

# Component(s) in the package.
atlas_add_library( xAODTrigCalo
   xAODTrigCalo/*.h xAODTrigCalo/versions/*.h Root/*.cxx
   PUBLIC_HEADERS xAODTrigCalo
   LINK_LIBRARIES CaloGeoHelpers AthContainers xAODCore )

atlas_add_xaod_smart_pointer_dicts(
   INPUT xAODTrigCalo/selection.xml
   OUTPUT _selectionFile
   CONTAINERS "xAOD::TrigCaloClusterContainer_v1"
              "xAOD::TrigEMClusterContainer_v1" )

atlas_add_dictionary( xAODTrigCaloDict
   xAODTrigCalo/xAODTrigCaloDict.h
   ${_selectionFile}
   LINK_LIBRARIES xAODCore xAODTrigCalo
   EXTRA_FILES Root/dict/*.cxx )

# Generate CLIDs for the library explicitly.
atlas_generate_cliddb( xAODTrigCalo )
