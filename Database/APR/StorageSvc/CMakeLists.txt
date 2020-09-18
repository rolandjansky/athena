# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( StorageSvc )

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
   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} AthContainers POOLCore )

atlas_add_dictionary( StorageSvcDict
   StorageSvc/dict.h StorageSvc/dict.xml
   LINK_LIBRARIES StorageSvc )

# The test(s) of the package:
atlas_add_test( StorageSvc_mtShape_test
   SOURCES test/StorageSvc_mtShape_test.cxx
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} StorageSvc
   POST_EXEC_SCRIPT nopost.sh )

atlas_add_test( StorageSvc_DbBlob_test
   SOURCES test/StorageSvc_DbBlob_test.cxx
   LINK_LIBRARIES StorageSvc )
