# $Id: CMakeLists.txt 744621 2016-05-03 18:38:44Z krasznaa $
################################################################################
# Package: xAODTriggerCnv
################################################################################

# Declare the package name:
atlas_subdir( xAODTriggerCnv )

# Extra dependencies, based on the build environment:
if( XAOD_STANDALONE )
   set( extra_deps PRIVATE Control/xAODRootAccess )
elseif( XAOD_ANALYSIS )
   set( extra_deps PRIVATE Control/xAODRootAccess Database/AthenaPOOL/AthenaPoolKernel )
else()
   set( extra_deps GaudiKernel
      PRIVATE
      Control/AthenaBaseComps
      Control/AthenaKernel
      Control/StoreGate
      Database/AthenaPOOL/AthenaPoolKernel
      Event/EventInfo
      PhysicsAnalysis/AnalysisTrigger/AnalysisTriggerEvent
      Trigger/TrigAnalysis/TrigAnalysisInterfaces
      Trigger/TrigConfiguration/TrigConfHLTData
      Trigger/TrigConfiguration/TrigConfInterfaces
      Trigger/TrigEvent/TrigDecisionEvent
      Trigger/TrigEvent/TrigDecisionInterface
      Trigger/TrigEvent/TrigSteeringEvent )
endif()

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   Control/AthToolSupport/AsgTools
   Event/xAOD/xAODTrigger
   ${extra_deps} )

# Build the package differently in standalone and Athena modes:
if( XAOD_ANALYSIS )

   # The main library is an installed one in this case:
   atlas_add_library( xAODTriggerCnvLib
      xAODTriggerCnv/*.h Root/*.cxx
      PUBLIC_HEADERS xAODTriggerCnv
      LINK_LIBRARIES AsgTools xAODTrigger )

   # Plus we also build a dictionary:
   atlas_add_dictionary( xAODTriggerCnvDict
      xAODTriggerCnv/xAODTriggerCnvDict.h
      xAODTriggerCnv/selection.xml
      LINK_LIBRARIES xAODTriggerCnvLib )

   # if AthAnalysis, need a component library too
   if( NOT XAOD_STANDALONE )
    atlas_add_component( xAODTriggerCnv
      src/components/*.cxx
      LINK_LIBRARIES xAODTriggerCnvLib )
   endif()

   # The test(s) need(s) ROOT:
   find_package( ROOT COMPONENTS Core Tree RIO )

   # Test(s) in standalone mode:
   if( XAOD_STANDALONE )
      atlas_add_test( ut_xaodtriggercnv_triggermenumetadatatool_test
         SOURCES test/ut_xaodtriggercnv_triggermenumetadatatool_test.cxx
         INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
         LINK_LIBRARIES ${ROOT_LIBRARIES} xAODRootAccess xAODTrigger
         xAODTriggerCnvLib )
   endif()

else()

   # The main library is a component one in this case:
   atlas_add_component( xAODTriggerCnv
      xAODTriggerCnv/*.h src/*.cxx Root/*.cxx src/components/*.cxx
      LINK_LIBRARIES AsgTools xAODTrigger GaudiKernel AthenaBaseComps
      AthenaKernel StoreGateLib SGtests EventInfo AnalysisTriggerEvent
      TrigConfHLTData TrigDecisionEvent TrigSteeringEvent )

endif()

# Install files from the package:
atlas_install_headers( xAODTriggerCnv )
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )
