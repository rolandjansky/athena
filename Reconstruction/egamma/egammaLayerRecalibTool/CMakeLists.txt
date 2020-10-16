# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( egammaLayerRecalibTool )

# External dependencies:
find_package( ROOT COMPONENTS Hist Core RIO )

# Component(s) in the package:
atlas_add_library( egammaLayerRecalibTool
   egammaLayerRecalibTool/*.h Root/*.cxx
   PUBLIC_HEADERS egammaLayerRecalibTool
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} AsgMessagingLib PATInterfaces AsgTools xAODCaloEvent xAODEgamma xAODEventInfo
   PRIVATE_LINK_LIBRARIES PathResolver )

atlas_add_dictionary( egammaLayerRecalibToolDict
   egammaLayerRecalibTool/egammaLayerRecalibToolDict.h
   egammaLayerRecalibTool/selection.xml
   LINK_LIBRARIES egammaLayerRecalibTool )

atlas_add_executable( egammaLayerRecalibTool_testmem
   util/egammaLayerRecalibTool_testmem.cxx
   LINK_LIBRARIES egammaLayerRecalibTool )

# Test(s) in the package:
if( XAOD_STANDALONE )

   atlas_add_test( ut_egammaLayerRecalibrationTool
      SCRIPT test/ut_egammaLayerRecalibrationTool.py )

endif()
