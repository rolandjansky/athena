# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( DataModelRoot )

# External dependencies:
find_package( ROOT COMPONENTS Core RIO )

# Component(s) in the package:
atlas_add_library( DataModelRoot
                   src/*.cxx
                   PUBLIC_HEADERS DataModelRoot
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${ROOT_LIBRARIES} CxxUtils
                   PRIVATE_LINK_LIBRARIES RootUtils )

atlas_add_dictionary( DataModelRootDict
                      DataModelRoot/DataModelRootDict.h
                      DataModelRoot/selection.xml
                      LINK_LIBRARIES DataModelRoot )
