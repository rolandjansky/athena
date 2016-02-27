# $Id: CMakeLists.txt 726582 2016-02-27 11:23:37Z krasznaa $
################################################################################
# Package: CollectionUtilities
################################################################################

# Declare the package name:
atlas_subdir( CollectionUtilities )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
  PUBLIC
  Database/APR/CollectionBase
  Database/APR/TrigCollQuery
  Database/PersistentDataModel
  PRIVATE
  Database/APR/FileCatalog
  Database/APR/POOLCore
  Database/APR/PersistencySvc )

# External dependencies:
find_package( Boost )
find_package( CORAL COMPONENTS CoralBase )
find_package( XercesC )
find_package( yampl )
find_package( CURL )

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
  TrigCollQueryLib
  PRIVATE_LINK_LIBRARIES ${XERCESC_LIBRARIES} ${Boost_LIBRARIES}
  FileCatalog POOLCore PersistencySvc )

# Helper macro declaring the utilities of the package:
macro( collection_utility name )
  atlas_add_executable( ${name} utilities/${name}.cpp
    INCLUDE_DIRS ${CURL_INCLUDE_DIRS} ${YAMPL_INCLUDE_DIRS}
    ${CORAL_INCLUDE_DIRS} ${XERCESC_INCLUDE_DIRS}
    LINK_LIBRARIES ${CURL_LIBRARIES} ${YAMPL_LIBRARIES}
    ${CORAL_LIBRARIES} ${XERCESC_LIBRARIES} CollectionBase
    PersistentDataModel FileCatalog POOLCore CollectionUtilities )
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
collection_utility( TokenExtractor )

# Install files from the package:
atlas_install_scripts( scripts/*.exe scripts/*.py )
