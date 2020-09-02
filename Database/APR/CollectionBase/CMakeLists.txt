# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( CollectionBase )

# External dependencies:
find_package( Boost )
find_package( CORAL COMPONENTS CoralBase )

# Component(s) in the package:
atlas_add_library( CollectionBase
                   src/*.cpp
                   PUBLIC_HEADERS CollectionBase
                   INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS}
                   LINK_LIBRARIES ${Boost_LIBRARIES} ${CORAL_LIBRARIES} FileCatalog
                   PRIVATE_LINK_LIBRARIES POOLCore PersistentDataModel )

atlas_add_dictionary( CollectionDict
                      CollectionBase/CollectionDict.h
                      CollectionBase/selection.xml
                      INCLUDE_DIRS ${CORAL_INCLUDE_DIRS}
                      LINK_LIBRARIES CollectionBase )

if( NOT GENERATIONBASE )
  atlas_add_test( Factory_test
                  SOURCES test/Factory_test.cxx
                  LINK_LIBRARIES CollectionBase POOLCore PersistentDataModel
                  PROPERTIES TIMEOUT 300 )
endif()
