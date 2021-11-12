# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( InDetSVWithMuonTool )

# External dependencies:
find_package( ROOT COMPONENTS Core MathCore Hist )

atlas_add_library(
    InDetSVWithMuonToolLib
    InDetSVWithMuonTool/*.h
    src/*.cxx

    PUBLIC_HEADERS
    InDetSVWithMuonTool

    PRIVATE_INCLUDE_DIRS
    ${ROOT_INCLUDE_DIRS}

    LINK_LIBRARIES
    AthenaBaseComps
    GaudiKernel
    TrkVertexFitterInterfaces
    xAODTracking

    PRIVATE_LINK_LIBRARIES
    ${ROOT_LIBRARIES}
    AnalysisUtilsLib
    TrkVKalVrtFitterLib
    xAODBTagging
    AthenaKernel
    CxxUtils
    )


# Component(s) in the package:
atlas_add_component(
    InDetSVWithMuonTool
    src/components/*.cxx

    LINK_LIBRARIES
    InDetSVWithMuonToolLib
    )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
