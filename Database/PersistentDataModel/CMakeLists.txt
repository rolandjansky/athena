# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( PersistentDataModel )

# External dependencies:
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )
find_package( UUID )

# Component(s) in the package:
atlas_add_library( PersistentDataModel
                   src/*.cxx
                   PUBLIC_HEADERS PersistentDataModel
                   INCLUDE_DIRS ${CORAL_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${UUID_INCLUDE_DIRS}
                   LINK_LIBRARIES ${CORAL_LIBRARIES} AthenaKernel CxxUtils GaudiKernel
                   PRIVATE_LINK_LIBRARIES ${UUID_LIBRARIES} SGTools )

atlas_add_dictionary( PersistentDataModelDict
                      PersistentDataModel/PersistentDataModelDict.h
                      PersistentDataModel/selection.xml
                      LINK_LIBRARIES PersistentDataModel )
