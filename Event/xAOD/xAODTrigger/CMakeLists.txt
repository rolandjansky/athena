# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name.
atlas_subdir( xAODTrigger )

# Extra dependencies, based on what environment we're in.
set( extra_libs )
if( NOT XAOD_STANDALONE )
   set( extra_libs AthenaKernel )
endif()

# Pull in the helper CMake code.
find_package( xAODUtilities )

# Component(s) in the package.
atlas_add_library( xAODTrigger
   xAODTrigger/*.h xAODTrigger/versions/*.h xAODTrigger/versions/*.icc
   Root/*.cxx
   PUBLIC_HEADERS xAODTrigger
   LINK_LIBRARIES AthContainers AthLinks xAODCore xAODBase TrigNavStructure CxxUtils
   ${extra_libs} )

atlas_add_xaod_smart_pointer_dicts(
   INPUT xAODTrigger/selection.xml
   OUTPUT _selectionFile
   CONTAINERS "xAOD::MuonRoIContainer_v1" "xAOD::JetRoIContainer_v1"
              "xAOD::JetRoIContainer_v2" "xAOD::EmTauRoIContainer_v1"
              "xAOD::EmTauRoIContainer_v2" "xAOD::TriggerMenuContainer_v1"
              "xAOD::TrigCompositeContainer_v1" "xAOD::BunchConfContainer_v1"
              "xAOD::TrigPassBitsContainer_v1" "xAOD::TriggerMenuJsonContainer_v1"
   OBJECTS "xAOD::EnergySumRoI_v1" "xAOD::EnergySumRoI_v2"
           "xAOD::TrigDecision_v1" "xAOD::TrigNavigation_v1"
           "xAOD::RoiDescriptorStore_v1" )

atlas_add_dictionary( xAODTriggerDict
   xAODTrigger/xAODTriggerDict.h
   ${_selectionFile}
   LINK_LIBRARIES xAODCore xAODTrigger
   EXTRA_FILES Root/dict/*.cxx )

atlas_add_test( ut_xaodtrigger_bytestreamauxcontainer_v1_test
  SOURCES test/ut_xaodtrigger_bytestreamauxcontainer_v1_test.cxx
  LINK_LIBRARIES AthContainers xAODTrigger )

if( NOT XAOD_STANDALONE )
   atlas_add_test( ut_xaodtrigger_trigcomposite_test
      SOURCES test/ut_xaodtrigger_trigcomposite_test.cxx
      LINK_LIBRARIES AthContainers xAODTrigger )
endif()
