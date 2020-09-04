# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration


# Declare the package name:
atlas_subdir( PoolSvc )

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
   LINK_LIBRARIES ${CORAL_LIBRARIES} AthenaBaseComps DBReplicaSvcLib FileCatalog POOLCore PathResolver PersistentDataModel PoolSvcLib StorageSvc )

# Install files from the package:
atlas_install_joboptions( share/*.py )
