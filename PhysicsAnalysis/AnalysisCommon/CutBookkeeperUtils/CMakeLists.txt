# Set the project's name and version.
atlas_subdir( CutBookkeeperUtils )

atlas_depends_on_subdirs(
  PUBLIC
  Control/xAODRootAccess
  Event/xAOD/xAODCore
  Event/xAOD/xAODCutFlow
  Control/AthToolSupport/AsgTools
  Control/AthToolSupport/AsgMessaging
  )

# Add the library
atlas_add_library(CutBookkeeperUtils
  Root/OriginalAodCounts.cxx
  Root/OriginalAodCountsTool.cxx
  PUBLIC_HEADERS
  CutBookkeeperUtils
  LINK_LIBRARIES
  xAODCore AsgTools xAODCutFlow xAODRootAccess AsgMessagingLib)

atlas_add_dictionary( CutBookkeeperUtilsDict
   CutBookkeeperUtils/CutBookkeeperUtilsDict.h
   CutBookkeeperUtils/selection.xml
   LINK_LIBRARIES CutBookkeeperUtils )

if( NOT XAOD_STANDALONE )
   atlas_add_component( CutBookkeeperUtilsLib
      src/*.h src/*.cxx src/components/*.cxx
      LINK_LIBRARIES GaudiKernel CutBookkeeperUtils )
endif()
