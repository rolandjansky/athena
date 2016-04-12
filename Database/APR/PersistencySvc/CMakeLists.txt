# $Id: CMakeLists.txt 739662 2016-04-12 11:55:10Z krasznaa $
################################################################################
# Package: PersistencySvc
################################################################################

# Declare the package name:
atlas_subdir( PersistencySvc )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   Control/DataModelRoot
   Database/APR/POOLCore
   Database/APR/StorageSvc
   PRIVATE
   Control/AthenaKernel
   Database/APR/FileCatalog
   Database/PersistentDataModel )

# External dependencies:
find_package( CORAL COMPONENTS CoralBase )

# Component(s) in the package:
atlas_add_library( PersistencySvc
   PersistencySvc/*.h PersistencySvc/*.inl
   src/*.cpp src/*.h src/*.inl
   PUBLIC_HEADERS PersistencySvc
   PRIVATE_INCLUDE_DIRS ${CORAL_INCLUDE_DIRS}
   LINK_LIBRARIES DataModelRoot POOLCore StorageSvc
   PRIVATE_LINK_LIBRARIES ${CORAL_LIBRARIES} AthenaKernel FileCatalog
   PersistentDataModel )

atlas_add_executable( pool_extractFileIdentifier
   utilities/pool_extractFileIdentifier.cpp
   LINK_LIBRARIES PersistencySvc )

atlas_add_executable( pool_insertFileToCatalog
   utilities/pool_insertFileToCatalog.cpp
   LINK_LIBRARIES FileCatalog PersistencySvc )
