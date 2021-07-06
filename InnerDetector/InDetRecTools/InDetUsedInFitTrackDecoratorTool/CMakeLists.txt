# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( InDetUsedInFitTrackDecoratorTool )

# Libraries in the package:
atlas_add_library( InDetUsedInFitTrackDecoratorToolLib
    Root/*.cxx
    PUBLIC_HEADERS InDetUsedInFitTrackDecoratorTool
    LINK_LIBRARIES AthLinks AsgDataHandlesLib AsgTools InDetRecToolInterfaces xAODTracking )

if( NOT XAOD_STANDALONE )
    atlas_add_component( InDetUsedInFitTrackDecoratorTool
        src/*.h src/*.cxx src/components/*.cxx
        LINK_LIBRARIES InDetUsedInFitTrackDecoratorToolLib )
endif()

atlas_add_dictionary( InDetUsedInFitTrackDecoratorToolDict
        InDetUsedInFitTrackDecoratorTool/InDetUsedInFitTrackDecoratorToolDict.h
    InDetUsedInFitTrackDecoratorTool/selection.xml
    LINK_LIBRARIES InDetUsedInFitTrackDecoratorToolLib )
