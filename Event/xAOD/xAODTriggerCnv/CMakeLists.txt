# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( xAODTriggerCnv )

# Extra dependencies, based on the build environment:
if( XAOD_STANDALONE )
   set( interface_extra_libs )
elseif( XAOD_ANALYSIS )
   set( interface_extra_libs GaudiKernel )
else()
   set( interface_extra_libs GaudiKernel )
endif()

atlas_add_library( xAODTriggerCnvLib
                   xAODTriggerCnv/*.h
                   INTERFACE
                   PUBLIC_HEADERS xAODTriggerCnv
                   LINK_LIBRARIES ${interface_extra_libs} xAODTrigger AsgTools )

# Build the package differently in standalone and Athena modes:
if( XAOD_STANDALONE )

   # The main library is an installed one in this case:
   atlas_add_library( xAODTriggerCnv
      xAODTriggerCnv/*.h Root/*.cxx
      PUBLIC_HEADERS xAODTriggerCnv
      LINK_LIBRARIES xAODTriggerCnvLib AsgTools xAODTrigger )

   # Plus we also build a dictionary:
   atlas_add_dictionary( xAODTriggerCnvDict
      xAODTriggerCnv/xAODTriggerCnvDict.h
      xAODTriggerCnv/selection.xml
      LINK_LIBRARIES xAODTriggerCnv )

   # The test(s) need(s) ROOT:
   find_package( ROOT COMPONENTS Core Tree RIO )

   # FIX ME: temporarily disabled as part of the migration of AnalysisBase
   #         to master

   # # Test(s) in standalone mode:
   # atlas_add_test( ut_xaodtriggercnv_triggermenumetadatatool_test
   #    SOURCES test/ut_xaodtriggercnv_triggermenumetadatatool_test.cxx
   #    INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   #    LINK_LIBRARIES ${ROOT_LIBRARIES} xAODRootAccess xAODTrigger
   #    xAODTriggerCnv )

elseif( XAOD_ANALYSIS )

   # The main library is a component one in this case:
   atlas_add_component( xAODTriggerCnv
      xAODTriggerCnv/*.h Root/*.cxx src/components/*.cxx
      LINK_LIBRARIES xAODTriggerCnvLib AsgTools xAODTrigger GaudiKernel )

else()

   # The main library is a component one in this case:
   atlas_add_component( xAODTriggerCnv
      xAODTriggerCnv/*.h src/*.cxx Root/*.cxx src/components/*.cxx
      LINK_LIBRARIES xAODTriggerCnvLib AsgTools xAODTrigger GaudiKernel AthenaBaseComps
      AthenaKernel StoreGateLib SGtests EventInfo AnalysisTriggerEvent
      TrigConfHLTData TrigDecisionEvent TrigSteeringEvent TrigConfInterfaces TrigDecisionInterface TrigAnalysisInterfaces )

endif()

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )
