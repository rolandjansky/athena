# $Id: CMakeLists.txt 780705 2016-10-27 11:45:18Z krasznaa $
################################################################################
# Package: TriggerMatchingTool
################################################################################

# Declare the package name:
atlas_subdir( TriggerMatchingTool )

# Extra dependencies, based on the build environment:
set( extra_deps )
if( NOT XAOD_STANDALONE )
   set( extra_deps Control/AthAnalysisBaseComps GaudiKernel )
endif()

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   Control/AthToolSupport/AsgTools
   Event/xAOD/xAODBase
   Trigger/TrigAnalysis/TrigDecisionTool
   Trigger/TrigEvent/TrigNavStructure
   PRIVATE
   Event/FourMomUtils
   Event/xAOD/xAODTrigger
   ${extra_deps} )

# External dependencies:
find_package( Boost )
find_package( ROOT COMPONENTS Core Hist RIO )

# Component(s) in the package:
atlas_add_library( TriggerMatchingToolLib
   TriggerMatchingTool/*.h Root/*.cxx
   PUBLIC_HEADERS TriggerMatchingTool
   PRIVATE_INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
   LINK_LIBRARIES AsgTools xAODBase TrigNavStructure TrigDecisionToolLib
   PRIVATE_LINK_LIBRARIES ${Boost_LIBRARIES} FourMomUtils xAODTrigger )

if( NOT XAOD_STANDALONE )
   atlas_add_component( TriggerMatchingTool
      src/*.h src/*.cxx src/components/*.cxx
      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
      LINK_LIBRARIES ${ROOT_LIBRARIES} xAODBase GaudiKernel
      AthAnalysisBaseCompsLib TriggerMatchingToolLib )
endif()

atlas_add_dictionary( TriggerMatchingToolDict
   TriggerMatchingTool/TriggerMatchingToolDict.h
   TriggerMatchingTool/selection.xml
   LINK_LIBRARIES TriggerMatchingToolLib )

# Install files from the package:
atlas_install_joboptions( share/*.py )
