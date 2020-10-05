# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
# Package: InDetBoundaryCheckTool

# Declare the package name
atlas_subdir(
    InDetBoundaryCheckTool
)

# Component(s) in the package:
atlas_add_component(
    InDetBoundaryCheckTool
    src/*.cxx
    src/components/*.cxx
    LINK_LIBRARIES 
    AthenaBaseComps 
    GaudiKernel 
    TrkParameters 
    TrkToolInterfaces
    TrkTrack
    InDetReadoutGeometry
    InDetConditionsSummaryService
    InDetRecToolInterfaces
)

# Install files from the package:
atlas_install_headers(
    InDetBoundaryCheckTool
)

