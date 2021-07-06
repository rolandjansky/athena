# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( CollectionBase )

# External dependencies:
find_package( Boost )
find_package( CORAL COMPONENTS CoralBase )
find_package( ROOT )

# Component(s) in the package:
atlas_add_library( CollectionBase
                   src/*.cpp
                   PUBLIC_HEADERS CollectionBase
                   INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS}
                   LINK_LIBRARIES ${Boost_LIBRARIES} ${CORAL_LIBRARIES} FileCatalog CxxUtils
                   PRIVATE_LINK_LIBRARIES POOLCore PersistentDataModel )

atlas_add_dictionary( CollectionDict
                      CollectionBase/CollectionDict.h
                      CollectionBase/selection.xml
                      INCLUDE_DIRS ${CORAL_INCLUDE_DIRS}
                      LINK_LIBRARIES CollectionBase )

if( NOT GENERATIONBASE )
  atlas_add_test( Factory_test
                  SOURCES test/Factory_test.cxx
                  INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                  LINK_LIBRARIES CollectionBase POOLCore PersistentDataModel
                  ${ROOT_LIBRARIES}
                  PROPERTIES TIMEOUT 300 )
endif()
