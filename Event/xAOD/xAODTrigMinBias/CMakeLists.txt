# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name.
atlas_subdir( xAODTrigMinBias )

# Pull in the helper CMake code.
find_package( xAODUtilities )

# Component(s) in the package.
atlas_add_library( xAODTrigMinBias
   xAODTrigMinBias/*.h xAODTrigMinBias/versions/*.h Root/*.cxx
   PUBLIC_HEADERS xAODTrigMinBias
   LINK_LIBRARIES AthContainers xAODCore )

atlas_add_xaod_smart_pointer_dicts(
   INPUT xAODTrigMinBias/selection.xml
   OUTPUT _selectionFile
   CONTAINERS "xAOD::TrigSpacePointCountsContainer_v1"
              "xAOD::TrigT2MbtsBitsContainer_v1" "xAOD::TrigHisto2DContainer_v1"
              "xAOD::TrigTrackCountsContainer_v1"
              "xAOD::TrigVertexCountsContainer_v1"
              "xAOD::TrigT2ZdcSignalsContainer_v1" )

atlas_add_dictionary( xAODTrigMinBiasDict
   xAODTrigMinBias/xAODTrigMinBiasDict.h
   ${_selectionFile}
   LINK_LIBRARIES xAODCore xAODTrigMinBias
   EXTRA_FILES Root/dict/*.cxx )

# Generate CLIDs from the library.
atlas_generate_cliddb( xAODTrigMinBias )
