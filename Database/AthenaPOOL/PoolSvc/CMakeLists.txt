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
   Database/ConnectionManagement/DBReplicaSvc
   Database/PersistentDataModel
   Tools/PathResolver )

# External dependencies:
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )

# Component(s) in the package:
atlas_add_component( PoolSvc
   PoolSvc/*.h src/*.cxx src/components/*.cxx
   INCLUDE_DIRS ${CORAL_INCLUDE_DIRS}
   LINK_LIBRARIES ${CORAL_LIBRARIES} DataModelRoot CollectionBase PersistencySvc GaudiKernel
   AthenaBaseComps AthenaKernel StorageSvc PersistentDataModel PathResolver )

# Install files from the package:
atlas_install_headers( PoolSvc )
atlas_install_joboptions( share/*.py )
