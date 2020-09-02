# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name.
atlas_subdir( RootFileTools )

# External dependencies:
find_package( ROOT COMPONENTS RIO Core Tree )

# Component(s) in the package:
atlas_add_executable( mergePOOL
   src/merge.cxx
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} PersistentDataModel )

atlas_add_executable( compressionTool
   src/compressionTool.cxx
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} )
