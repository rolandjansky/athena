# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( AthenaPoolMultiTest )

# External dependencies:
find_package( CORAL COMPONENTS CoralBase )

# Component(s) in the package:
atlas_add_component( AthenaPoolMultiTest
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${CORAL_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CORAL_LIBRARIES} AthenaBaseComps AthenaPoolExampleData AthenaPoolTestData AthenaPoolUtilities DBDataModel GaudiKernel GoodRunsListsLib PersistentDataModel SGTools StoreGateLib xAODEventInfo )

# Install files from the package:
atlas_install_joboptions( share/*.py )

# Tests in the package:
atlas_add_test( AthenaPoolMultiTestBuildInput
                SCRIPT athena.py AthenaPoolMultiTest/SplittableData_jo.py
                LOG_SELECT_PATTERN "WriteData|AddTrigMap|Stream1|FullColl|NullColl"
                LOG_IGNORE_PATTERN "ExampleHit"
                PROPERTIES TIMEOUT 600 )

atlas_add_test( AthenaPoolMultiTestEventSplit
                SCRIPT athena.py AthenaPoolMultiTest/EventSplit_jo.py
                LOG_SELECT_PATTERN "Splitter|Triggered|EventInfoTag|Stream|Coll|Others|Bad"
                PROPERTIES DEPENDS AthenaPoolMultiTest_AthenaPoolMultiTestBuildInput_ctest
                PROPERTIES TIMEOUT 600 )

atlas_add_test( AthenaPoolMultiTestEventSplitDecision
                SCRIPT athena.py AthenaPoolMultiTest/EventSplitDecision_jo.py
                LOG_SELECT_PATTERN "Splitter|Triggered|EventInfoTag|Stream|Coll|Others|Bad"
                PROPERTIES DEPENDS AthenaPoolMultiTest_AthenaPoolMultiTestBuildInput_ctest
                PROPERTIES TIMEOUT 600 )

atlas_add_test( AthenaPoolMultiTestEventNoSplit
                SCRIPT athena.py AthenaPoolMultiTest/EventNoSplit_jo.py
                LOG_SELECT_PATTERN "Splitter|Triggered|EventInfoTag|Stream|Coll|Others|Bad"
                PROPERTIES DEPENDS AthenaPoolMultiTest_AthenaPoolMultiTestBuildInput_ctest
                PROPERTIES TIMEOUT 600 )

atlas_add_test( AthenaPoolMultiTestPropagate
                SCRIPT athena.py AthenaPoolMultiTest/DecisionPropagation_jo.py
                LOG_SELECT_PATTERN "Splitter|Triggered|EventInfoTag|Stream|Coll|Others|Bad"
                PROPERTIES DEPENDS AthenaPoolMultiTest_AthenaPoolMultiTestEventSplitDecision_ctest
                PROPERTIES TIMEOUT 600 )

