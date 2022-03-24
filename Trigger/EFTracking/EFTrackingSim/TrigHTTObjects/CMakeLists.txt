# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigHTTObjects )

# External dependencies:
find_package( ROOT COMPONENTS Core MathCore )

# Component(s) in the package:
atlas_add_root_dictionary( TrigHTTObjectsLib
   TrigHTTObjectsLibDictSource
   ROOT_HEADERS            TrigHTTObjects/*.h
   EXTERNAL_PACKAGES       ROOT
)

atlas_add_library( TrigHTTObjectsLib
   src/*.cxx TrigHTTObjects/*.h ${TrigHTTObjectsLibDictSource}
   PUBLIC_HEADERS          TrigHTTObjects
   INCLUDE_DIRS            ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES          ${ROOT_LIBRARIES}
   PRIVATE_LINK_LIBRARIES  AsgMessagingLib
)

# Tests in the package:
atlas_add_test(             TrigHTTRoad_test
    SOURCES                 test/HTTRoad_test.cxx
    LINK_LIBRARIES          TrigHTTObjectsLib
)

atlas_add_test(             TrigHTTTrack_test
    SOURCES                 test/HTTTrack_test.cxx
    LINK_LIBRARIES          TrigHTTObjectsLib
)

atlas_add_test(             TrigHTTLogicalEventInputHeader_test
    SOURCES                 test/HTTLogicalEventInputHeader_test.cxx
    LINK_LIBRARIES          TrigHTTObjectsLib
)

atlas_add_test(             TrigHTTLogicalEventOutputHeader_test
    SOURCES                 test/HTTLogicalEventOutputHeader_test.cxx
    LINK_LIBRARIES          TrigHTTObjectsLib
)
