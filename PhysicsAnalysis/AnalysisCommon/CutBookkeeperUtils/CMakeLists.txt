# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Set the project's name and version.
atlas_subdir( CutBookkeeperUtils )

# External dependencies:
find_package( ROOT COMPONENTS Core RIO )

# Add the library
atlas_add_library(CutBookkeeperUtils
  Root/OriginalAodCounts.cxx
  PUBLIC_HEADERS CutBookkeeperUtils
  PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
  PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} xAODCutFlow xAODRootAccess )
