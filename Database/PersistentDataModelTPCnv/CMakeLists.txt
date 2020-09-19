# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( PersistentDataModelTPCnv )

# External dependencies:
find_package( UUID )

# Component(s) in the package:
atlas_add_library( PersistentDataModelTPCnv
                   src/*.cxx
                   PUBLIC_HEADERS PersistentDataModelTPCnv
                   PRIVATE_INCLUDE_DIRS ${UUID_INCLUDE_DIRS}
                   LINK_LIBRARIES PersistentDataModel
                   PRIVATE_LINK_LIBRARIES ${UUID_LIBRARIES} CxxUtils )

atlas_add_dictionary( PersistentDataModelTPCnvDict
                      PersistentDataModelTPCnv/PersistentDataModelTPCnvDict.h
                      PersistentDataModelTPCnv/selection.xml
                      LINK_LIBRARIES PersistentDataModelTPCnv )
