# $Id: CMakeLists.txt 797235 2017-02-14 20:30:30Z ssnyder $
################################################################################
# Package: xAODTrigger
################################################################################

# Declare the package name:
atlas_subdir( xAODTrigger )

# Extra dependencies, based on what environment we're in:
if( NOT XAOD_STANDALONE )
   set( extra_deps Control/AthenaKernel )
   set( extra_libs AthenaKernel )
endif()

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   Control/AthContainers
   Control/AthContainersInterfaces
   Control/AthLinks
   Event/xAOD/xAODCore
   ${extra_deps} )

# Component(s) in the package:
atlas_add_library( xAODTrigger
   xAODTrigger/*.h xAODTrigger/versions/*.h xAODTrigger/versions/*.icc
   Root/*.cxx
   PUBLIC_HEADERS xAODTrigger
   LINK_LIBRARIES AthContainers AthLinks xAODCore ${extra_libs} )

atlas_add_dictionary( xAODTriggerDict
   xAODTrigger/xAODTriggerDict.h
   xAODTrigger/selection.xml
   LINK_LIBRARIES xAODTrigger
   EXTRA_FILES Root/dict/*.cxx )

atlas_add_test( ut_xaodtrigger_bytestreamauxcontainer_v1_test
  SOURCES test/ut_xaodtrigger_bytestreamauxcontainer_v1_test.cxx
  LINK_LIBRARIES AthContainers xAODTrigger )
