# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( CollectionUtilities )

# External dependencies:
find_package( Boost )
find_package( CORAL COMPONENTS CoralBase )
find_package( XercesC )

# Component(s) in the package:
atlas_add_library( CollectionUtilities
  src/ArgQual.cpp
  src/CmdLineArgs2.cpp
  src/Args2Container.cpp
  src/CollectionMetadataParser.cpp
  src/CatalogInfo.cpp
  src/CollectionPool.cpp
  src/CollSplitByGUIDBase.cpp
  src/Progress.cpp
  src/CollAppendBase.cpp
  src/UtilityFuncs.cpp
  src/MaxEventsInfo.cpp
  src/QueryInfo.cpp
  src/MetaInfo.cpp
  src/GenericMetaHandler.cpp
  src/CollMetaRegistry.cpp
  PUBLIC_HEADERS CollectionUtilities
  INCLUDE_DIRS ${CORAL_INCLUDE_DIRS}
  PRIVATE_INCLUDE_DIRS ${XERCESC_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS}
  LINK_LIBRARIES ${CORAL_LIBRARIES} CollectionBase PersistentDataModel
  PRIVATE_LINK_LIBRARIES ${XERCESC_LIBRARIES} ${Boost_LIBRARIES}
  FileCatalog POOLCore )

# Helper macro declaring the utilities of the package:
macro( collection_utility name )
  atlas_add_executable( ${name} utilities/${name}.cpp
    INCLUDE_DIRS $${CORAL_INCLUDE_DIRS} ${XERCESC_INCLUDE_DIRS}
    LINK_LIBRARIES ${CORAL_LIBRARIES} ${XERCESC_LIBRARIES} CollectionBase
    PersistentDataModel PersistencySvc FileCatalog POOLCore CollectionUtilities )
endmacro( collection_utility )

# Declare the utilities of the package:
collection_utility( coll_insertGuidToCatalog )
collection_utility( CollAppend )
collection_utility( CollListAttrib )
collection_utility( CollListFileGUID )
collection_utility( CollListMetadata )
collection_utility( CollListPFN )
collection_utility( CollListToken )
collection_utility( CollQuery )
collection_utility( CollRemove )
collection_utility( CollSplitByGUID )

# Install files from the package:
atlas_install_scripts( scripts/*.exe scripts/*.py )
