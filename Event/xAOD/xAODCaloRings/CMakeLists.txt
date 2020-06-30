# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name.
atlas_subdir( xAODCaloRings )

# Pull in the helper CMake code.
find_package( xAODUtilities )

# Component(s) in the package.
atlas_add_library( xAODCaloRings
   xAODCaloRings/*.h xAODCaloRings/versions/*.h Root/*.cxx
   PUBLIC_HEADERS xAODCaloRings
   LINK_LIBRARIES CaloGeoHelpers AthContainers AthLinks xAODCore )

atlas_add_xaod_smart_pointer_dicts(
   INPUT xAODCaloRings/selection.xml
   OUTPUT _selectionFile
   CONTAINERS "xAOD::RingSetContainer_v1" "xAOD::RingSetConfContainer_v1"
              "xAOD::CaloRingsContainer_v1" )

atlas_add_dictionary( xAODCaloRingsDict
   xAODCaloRings/xAODCaloRingsDict.h
   ${_selectionFile}
   LINK_LIBRARIES xAODCore xAODCaloRings
   EXTRA_FILES Root/dict/*.cxx )

# Generate CLIDs for the library explicitly.
atlas_generate_cliddb( xAODCaloRings )
