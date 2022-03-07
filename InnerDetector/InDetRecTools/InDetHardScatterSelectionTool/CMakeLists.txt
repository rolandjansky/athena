# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( InDetHardScatterSelectionTool )

# Libraries in the package:
atlas_add_library( InDetHardScatterSelectionToolLib
    Root/*.cxx
    PUBLIC_HEADERS InDetHardScatterSelectionTool
    LINK_LIBRARIES AthLinks AsgDataHandlesLib AsgTools GaudiKernel InDetRecToolInterfaces InDetTrackSelectionToolLib xAODTracking xAODJet FourMomUtils)

if( NOT XAOD_STANDALONE )
    atlas_add_component( InDetHardScatterSelectionTool
        src/*.h src/*.cxx src/components/*.cxx
        LINK_LIBRARIES InDetHardScatterSelectionToolLib )
endif()

atlas_add_dictionary( InDetHardScatterSelectionToolDict
    InDetHardScatterSelectionTool/InDetHardScatterSelectionToolDict.h
    InDetHardScatterSelectionTool/selection.xml
    LINK_LIBRARIES InDetHardScatterSelectionToolLib )
