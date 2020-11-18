# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( HIEventUtils )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( HIEventUtilsLib
                   Root/*.cxx
                   PUBLIC_HEADERS HIEventUtils
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
		           LINK_LIBRARIES ${ROOT_LIBRARIES} CaloGeoHelpers AsgTools xAODForward xAODHIEvent xAODTracking xAODTrigL1Calo PATCoreAcceptLib InDetTrackSelectionToolLib
                   PRIVATE_LINK_LIBRARIES AsgMessagingLib CxxUtils PathResolver xAODEventInfo )

if (NOT XAOD_STANDALONE)
  atlas_add_component( HIEventUtils
                       src/components/*.cxx
                       LINK_LIBRARIES HIEventUtilsLib )
endif ()

# Install files from the package:
atlas_install_joboptions( share/*.py )
atlas_install_data( share/*.root )

