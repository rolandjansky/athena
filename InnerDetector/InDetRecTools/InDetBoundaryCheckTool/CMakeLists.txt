# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
# Package: InDetBoundaryCheckTool

# Declare the package name
atlas_subdir(
    InDetBoundaryCheckTool
)

# Declare the package's dependencies
atlas_depends_on_subdirs( 
    PUBLIC
    Control/AthenaBaseComps
    Tracking/TrkTools/TrkToolInterfaces
    InnerDetector/InDetConditions/InDetConditionsSummaryService
    InnerDetector/InDetDetDescr/InDetReadoutGeometry
    InnerDetector/InDetRecToolInterfaces
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
)

# Install files from the package:
atlas_install_headers(
    InDetBoundaryCheckTool
)

