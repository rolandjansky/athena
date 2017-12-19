# $Id: CMakeLists.txt 739318 2016-04-11 12:41:08Z krasznaa $
################################################################################
# Package: StorageSvc
################################################################################

# Declare the package name:
atlas_subdir( StorageSvc )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   Control/CxxUtils
   Control/DataModelRoot
   Database/PersistentDataModel
   PRIVATE
   AtlasTest/TestTools
   Control/AthContainers
   GaudiKernel )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO Thread pthread )
find_package( UUID )

# Component(s) in the package:
atlas_add_library( StorageSvc src/*.cpp
   PUBLIC_HEADERS StorageSvc
   INCLUDE_DIRS ${UUID_INCLUDE_DIRS}
   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${UUID_LIBRARIES} CxxUtils DataModelRoot
   PersistentDataModel
   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} TestTools AthContainers
   GaudiKernel )

atlas_add_dictionary( StorageSvcDict StorageSvc/dict.h StorageSvc/dict.xml
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${UUID_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} ${UUID_LIBRARIES}
   CxxUtils DataModelRoot PersistentDataModel TestTools AthContainers
   GaudiKernel StorageSvc )

# The test(s) of the package:
atlas_add_test( StorageSvc_mtShape_test
   SOURCES test/StorageSvc_mtShape_test.cxx
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} StorageSvc
   EXTRA_PATTERNS ".*" )

atlas_add_test( StorageSvc_DbBlob_test
   SOURCES test/StorageSvc_DbBlob_test.cxx
   LINK_LIBRARIES StorageSvc )
