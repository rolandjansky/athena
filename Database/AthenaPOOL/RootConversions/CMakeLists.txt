# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( RootConversions )

# External dependencies:
find_package( ROOT COMPONENTS RIO Tree TreePlayer Core MathCore Hist pthread )

# Component(s) in the package:
atlas_add_library( RootConversions
                   src/*.cxx
                   PUBLIC_HEADERS RootConversions
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} TestTools RootUtils )

atlas_add_dictionary( RootConversionsDict
                      RootConversions/RootConversionsDict.h
                      RootConversions/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} RootConversions )

atlas_add_dictionary( TestRootConversions1Dict
                      test/RootConversionsTest1Dict.h
                      test/selection_test1.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} RootConversions
                      NO_ROOTMAP_MERGE )

atlas_add_dictionary( TestRootConversions2Dict
                      test/RootConversionsTest2Dict.h
                      test/selection_test2.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} RootConversions
                      NO_ROOTMAP_MERGE )

