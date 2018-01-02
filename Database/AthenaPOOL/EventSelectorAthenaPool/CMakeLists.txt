################################################################################
# Package: EventSelectorAthenaPool
################################################################################

# Declare the package name:
atlas_subdir( EventSelectorAthenaPool )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Control/StoreGate
                          Database/APR/CollectionBase
                          Database/APR/PersistencySvc
                          Database/AthenaPOOL/AthenaPoolCnvSvc
                          Database/AthenaPOOL/AthenaPoolUtilities
                          Database/AthenaPOOL/DBDataModel
                          Database/AthenaPOOL/PoolSvc
                          Database/PersistentDataModel
                          GaudiKernel )

# External dependencies:
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )

# Component(s) in the package:
atlas_add_component( EventSelectorAthenaPool
                     src/AthenaPoolAddressProviderSvc.cxx
                     src/CondProxyProvider.cxx
                     src/EventSelectorAthenaPool.cxx
                     src/PoolCollectionConverter.cxx
                     src/registerKeys.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${CORAL_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CORAL_LIBRARIES} AthenaBaseComps AthenaKernel StoreGateLib SGtests CollectionBase PersistencySvc AthenaPoolCnvSvcLib AthenaPoolUtilities DBDataModel PersistentDataModel GaudiKernel )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

