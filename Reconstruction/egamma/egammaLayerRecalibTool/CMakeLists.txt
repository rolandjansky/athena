# $Id: CMakeLists.txt 776712 2016-10-05 16:37:44Z krasznaa $
################################################################################
# Package: egammaLayerRecalibTool
###############################################################################

# Declare the package name:
atlas_subdir( egammaLayerRecalibTool )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   PhysicsAnalysis/AnalysisCommon/PATInterfaces
   Control/AthToolSupport/AsgTools
   Event/xAOD/xAODCaloEvent
   Event/xAOD/xAODEgamma
	Event/xAOD/xAODEventInfo
   PRIVATE
   Tools/PathResolver )

# External dependencies:
find_package( ROOT COMPONENTS Hist Core RIO )

# Component(s) in the package:
atlas_add_library( egammaLayerRecalibTool
   egammaLayerRecalibTool/*.h Root/*.cxx
   PUBLIC_HEADERS egammaLayerRecalibTool
	PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES PATInterfaces AsgTools xAODCaloEvent xAODEgamma xAODEventInfo
   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} PathResolver )

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
