# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name.
atlas_subdir( xAODEventInfo )

# External dependencies.
find_package( ROOT COMPONENTS Core Tree RIO )
find_package( xAODUtilities )

# Component(s) in the package.
atlas_add_library( xAODEventInfo
   xAODEventInfo/versions/*.h xAODEventInfo/*.h Root/*.cxx
   PUBLIC_HEADERS xAODEventInfo
   LINK_LIBRARIES AthContainers AthLinks xAODCore )

atlas_add_xaod_smart_pointer_dicts(
   INPUT xAODEventInfo/selection.xml
   OUTPUT _selectionFile
   CONTAINERS "xAOD::EventInfoContainer_v1"
   OBJECTS "xAOD::EventInfo_v1" )

atlas_add_dictionary( xAODEventInfoDict
   xAODEventInfo/xAODEventInfoDict.h
   ${_selectionFile}
   LINK_LIBRARIES xAODCore xAODEventInfo
   EXTRA_FILES Root/dict/*.cxx )

# Test(s) in the package.
atlas_add_test( ut_xaodeventinfo_evtstore_test
   SOURCES test/ut_xaodeventinfo_evtstore_test.cxx
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} xAODEventInfo
   PROPERTIES TIMEOUT 300
   LOG_IGNORE_PATTERN "[0-9]+ bytes" )

atlas_add_test( ut_xaodeventinfo_printop_test
   SOURCES test/ut_xaodeventinfo_printop_test.cxx
   LINK_LIBRARIES xAODEventInfo )

atlas_add_test( ut_xaodeventinfo_subevent_test
   SOURCES test/ut_xaodeventinfo_subevent_test.cxx
   LINK_LIBRARIES AthLinks xAODCore xAODEventInfo )

atlas_add_test( ut_xaodeventinfo_eventauxinfo_test
   SOURCES test/ut_xaodeventinfo_eventauxinfo_test.cxx
   LINK_LIBRARIES AthContainers TestTools xAODEventInfo )

atlas_add_test( ut_xaodeventinfo_eventinfoauxcontainer_v1_test
   SOURCES test/ut_xaodeventinfo_eventinfoauxcontainer_v1_test.cxx
   LINK_LIBRARIES AthContainers TestTools xAODEventInfo )

atlas_add_test( ut_xaodeventinfo_eventinfo_v1_test
   SOURCES test/ut_xaodeventinfo_eventinfo_v1_test.cxx
   LINK_LIBRARIES xAODEventInfo )
