# Set the project's name and version.
atlas_subdir( CutBookkeeperUtils )

atlas_depends_on_subdirs(
  PUBLIC
  Control/xAODRootAccess
  Event/xAOD/xAODCore
  Event/xAOD/xAODCutFlow
  Control/AthToolSupport/AsgMessaging
  )

# Add the hdf tuple library
atlas_add_library(CutBookkeeperUtils
  Root/OriginalAodCounts.cxx
  PUBLIC_HEADERS CutBookKeeperUtils
  LINK_LIBRARIES xAODCore xAODCutFlow xAODRootAccess AsgMessagingLib)
