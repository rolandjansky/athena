# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( EventSelectorAthenaPool )

# External dependencies:
find_package( CORAL COMPONENTS CoralBase )

# Component(s) in the package:
atlas_add_component( EventSelectorAthenaPool
                     src/AthenaPoolAddressProviderSvc.cxx
                     src/CondProxyProvider.cxx
                     src/DoubleEventSelectorAthenaPool.cxx
                     src/EventSelectorAthenaPool.cxx
                     src/PoolCollectionConverter.cxx
                     src/registerKeys.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${CORAL_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CORAL_LIBRARIES} AthenaBaseComps AthenaKernel AthenaPoolCnvSvcLib AthenaPoolUtilities CollectionBase GaudiKernel PersistencySvc PersistentDataModel PoolSvcLib StoreGateLib )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/*.py )

atlas_add_test( CondProxyProviderConfig_test
                SCRIPT python -m EventSelectorAthenaPool.CondProxyProviderConfig
                LOG_SELECT_PATTERN "ComponentAccumulator|^---|^CondProxyProvider" )

