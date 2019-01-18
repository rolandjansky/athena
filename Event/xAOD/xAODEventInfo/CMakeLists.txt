# $Id: CMakeLists.txt 744185 2016-05-02 09:31:12Z krasznaa $
################################################################################
# Package: xAODEventInfo
################################################################################

# Declare the package name:
atlas_subdir( xAODEventInfo )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   Control/AthContainers
   Control/AthLinks
   Control/AthenaBaseComps
   Event/xAOD/xAODCore )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree RIO )

# Component(s) in the package:
atlas_add_library( xAODEventInfo
   xAODEventInfo/*.h Root/*.cxx
   PUBLIC_HEADERS xAODEventInfo
   LINK_LIBRARIES AthContainers AthLinks AthenaBaseComps xAODCore )

atlas_add_dictionary( xAODEventInfoDict
   xAODEventInfo/xAODEventInfoDict.h
   xAODEventInfo/selection.xml
   LINK_LIBRARIES xAODEventInfo
   EXTRA_FILES Root/dict/*.cxx )

# Test(s) in the package:
atlas_add_test( ut_xaodeventinfo_evtstore_test
   SOURCES test/ut_xaodeventinfo_evtstore_test.cxx
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} xAODEventInfo
   EXTRA_PATTERNS "<TInterpreter::ReadRootmapFile>|[0-9]+ bytes" )

atlas_add_test( ut_xaodeventinfo_printop_test
   SOURCES test/ut_xaodeventinfo_printop_test.cxx
   LINK_LIBRARIES xAODEventInfo )

atlas_add_test( ut_xaodeventinfo_subevent_test
   SOURCES test/ut_xaodeventinfo_subevent_test.cxx
   LINK_LIBRARIES AthLinks xAODCore xAODEventInfo )
