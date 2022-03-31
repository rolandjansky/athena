# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( InDetUsedInVertexFitTrackDecorator )

# Libraries in the package:
atlas_add_library( InDetUsedInVertexFitTrackDecoratorLib
    src/*.cxx
    PUBLIC_HEADERS InDetUsedInVertexFitTrackDecorator
    LINK_LIBRARIES AthenaBaseComps GaudiKernel InDetRecToolInterfaces )

atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )

if( NOT XAOD_STANDALONE )
    atlas_add_component( InDetUsedInVertexFitTrackDecorator
        src/components/*.cxx
        LINK_LIBRARIES InDetUsedInVertexFitTrackDecoratorLib )
endif()
