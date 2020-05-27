# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name.
atlas_subdir( xAODCaloEvent )

# Extra dependencies and sources, based on what environment we're in.
set( extra_libs )
set( extra_sources )
if( NOT XAOD_ANALYSIS AND NOT SIMULATIONBASE AND NOT GENERATIONBASE )
   set( extra_libs CaloEvent )
   set( extra_sources src/*.cxx )
endif()

# External dependencies.
find_package( ROOT COMPONENTS Core GenVector )
find_package( xAODUtilities )

# Component(s) in the package:
atlas_add_library( xAODCaloEvent
   xAODCaloEvent/*.h xAODCaloEvent/versions/*.h Root/*.cxx ${extra_sources}
   PUBLIC_HEADERS xAODCaloEvent
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} CaloGeoHelpers AthContainers
   CxxUtils GeoPrimitives xAODBase xAODCore AthLinks ${extra_libs} )

atlas_add_xaod_smart_pointer_dicts(
   INPUT xAODCaloEvent/selection.xml
   OUTPUT _selectionFile
   CONTAINERS "xAOD::CaloClusterContainer_v1" "xAOD::CaloTowerContainer_v1" )

atlas_add_dictionary( xAODCaloEventDict
   xAODCaloEvent/xAODCaloEventDict.h
   ${_selectionFile}
   LINK_LIBRARIES xAODCore xAODCaloEvent
   EXTRA_FILES Root/dict/*.cxx )

# Generate CLIDs for the library explicitly.
atlas_generate_cliddb( xAODCaloEvent )
