# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( RootStorageSvc )

# External dependencies:
find_package( ROOT COMPONENTS Core RIO TreePlayer Tree MathCore Hist pthread )

# Component(s) in the package:
atlas_add_library( RootStorageSvc
                   src/*.cpp
                   NO_PUBLIC_HEADERS
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} AthContainers AthContainersInterfaces
                   RootUtils RootUtilsPyROOT StorageSvc RootAuxDynIO POOLCore GaudiKernel
                   CxxUtils )

# Component list generation:
atlas_generate_componentslist( RootStorageSvc )


atlas_add_test( branchContIdx_test
   SOURCES test/branchContIdx_test.cxx
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} PersistencySvc StorageSvc AthContainers
   LOG_IGNORE_PATTERN "Token for .*"
)

