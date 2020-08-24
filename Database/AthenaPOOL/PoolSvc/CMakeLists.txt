# $Id: CMakeLists.txt 739587 2016-04-12 08:08:58Z krasznaa $
################################################################################
# Package: PoolSvc
################################################################################

# Declare the package name:
atlas_subdir( PoolSvc )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   Control/DataModelRoot
   Database/APR/CollectionBase
   Database/APR/PersistencySvc
   GaudiKernel
   PRIVATE
   Control/AthenaBaseComps
   Control/AthenaKernel
   Database/APR/StorageSvc
   Database/APR/FileCatalog
   Database/ConnectionManagement/DBReplicaSvc
   Database/PersistentDataModel
   Tools/PathResolver )

# External dependencies:
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )

atlas_add_library( PoolSvcLib
                   INTERFACE
                   PUBLIC_HEADERS PoolSvc
                   LINK_LIBRARIES GaudiKernel CollectionBase PersistencySvc DataModelRoot )

# Component(s) in the package:
atlas_add_component( PoolSvc
   PoolSvc/*.h src/*.cxx src/components/*.cxx
   INCLUDE_DIRS ${CORAL_INCLUDE_DIRS}
   LINK_LIBRARIES ${CORAL_LIBRARIES} DataModelRoot CollectionBase
   PersistencySvc GaudiKernel FileCatalog
   AthenaBaseComps AthenaKernel StorageSvc PersistentDataModel PathResolver DBReplicaSvcLib )

# Install files from the package:
atlas_install_joboptions( share/*.py )
