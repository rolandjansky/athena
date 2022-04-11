# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( InDetBoundaryCheckTool )

# Component(s) in the package:
atlas_add_component(
    InDetBoundaryCheckTool
    src/*.cxx
    src/components/*.cxx
    LINK_LIBRARIES 
    AthenaBaseComps
    GeoModelInterfaces
    TrkParameters
    TrkToolInterfaces
    InDetReadoutGeometry
    InDetConditionsSummaryService
    InDetRecToolInterfaces
    PRIVATE_LINK_LIBRARIES
    SCT_ReadoutGeometry
)
