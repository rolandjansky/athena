# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( RootCollection )

# External dependencies:
find_package( CORAL COMPONENTS CoralBase )
find_package( ROOT COMPONENTS MathCore Graf Hist Tree TreePlayer Net RIO Cint Core pthread )

# Component(s) in the package:
atlas_add_root_dictionary( RootCollection
                           RootCollectionDictSource
                           ROOT_HEADERS RootCollection/AttributeListLayout.h RootCollection/LinkDef.h
                           EXTERNAL_PACKAGES ROOT CORAL )

atlas_add_library( RootCollection
                   src/*.cpp
                   ${RootCollectionDictSource}
                   PUBLIC_HEADERS RootCollection
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${CORAL_INCLUDE_DIRS}
                   LINK_LIBRARIES ${ROOT_LIBRARIES} RootUtilsPyROOT
                   PRIVATE_LINK_LIBRARIES ${CORAL_LIBRARIES} AthenaKernel CollectionBase
                   FileCatalog GaudiKernel POOLCore PersistencySvc PersistentDataModel RootUtils )

atlas_add_library( RootCollectionComponents
                   NO_PUBLIC_HEADERS
                   src/components/*.cpp
                   LINK_LIBRARIES CollectionBase RootCollection )

atlas_add_test( read_test
                SOURCES
                test/read_test.cxx
                LINK_LIBRARIES ${CORAL_LIBRARIES} CollectionBase PersistentDataModel RootCollection )

atlas_add_test( update_test
                SOURCES
                test/update_test.cxx
                LINK_LIBRARIES ${CORAL_LIBRARIES} CollectionBase PersistentDataModel RootCollection )

set_tests_properties( RootCollection_update_test_ctest
                      PROPERTIES DEPENDS RootCollection_read_test_ctest )

# Component list generation:
atlas_generate_componentslist( RootCollectionComponents )
