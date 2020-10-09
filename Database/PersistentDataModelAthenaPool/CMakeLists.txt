# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( PersistentDataModelAthenaPool )

# External dependencies:
find_package( CORAL COMPONENTS CoralBase )

# Component(s) in the package:
atlas_add_poolcnv_library( PersistentDataModelAthenaPoolPoolCnv
                           src/*.cxx
                           FILES PersistentDataModel/DataHeader.h
                           INCLUDE_DIRS ${CORAL_INCLUDE_DIRS}
                           LINK_LIBRARIES ${CORAL_LIBRARIES} AthenaBaseComps AthenaPoolCnvSvcLib GaudiKernel PersistentDataModel PersistentDataModelTPCnv )
