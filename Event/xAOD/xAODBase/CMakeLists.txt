# $Id: CMakeLists.txt 744422 2016-05-03 11:34:39Z krasznaa $
################################################################################
# Package: xAODBase
################################################################################

# Declare the package name:
atlas_subdir( xAODBase )

# Extra dependencies based on what environment we are in:
if( NOT XAOD_STANDALONE )
   set( extra_deps Control/SGTools )
   set( extra_libs SGTools )
endif()

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   Control/AthContainers
   ${extra_deps}
   PRIVATE
   Control/AthLinks )

# External dependencies:
find_package( ROOT COMPONENTS Core Physics )

# Component(s) in the package:
atlas_add_library( xAODBase
   xAODBase/*.h Root/*.cxx
   PUBLIC_HEADERS xAODBase
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} AthContainers ${extra_libs}
   PRIVATE_LINK_LIBRARIES AthLinks )

atlas_add_dictionary( xAODBaseDict
   xAODBase/xAODBaseDict.h
   xAODBase/selection.xml
   LINK_LIBRARIES xAODBase )

atlas_add_dictionary( xAODBaseObjectTypeDict
   xAODBase/xAODBaseObjectTypeDict.h
   xAODBase/selection-ObjectType.xml)

# Test(s) in the package:
atlas_add_test( ut_xAODObjectType_test
   SOURCES test/ut_xAODObjectType_test.cxx
   LINK_LIBRARIES xAODBase )
