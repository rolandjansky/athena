################################################################################
# Package: TrkGlobalChi2Fitter
################################################################################

# Declare the package name:
atlas_subdir(TrkGlobalChi2Fitter)

# Define the library:
atlas_add_library(
    TrkGlobalChi2FitterLib
    PUBLIC_HEADERS
    TrkGlobalChi2Fitter
    LINK_LIBRARIES
    AthenaBaseComps
    GaudiKernel
    TrkEventPrimitives
    TrkMaterialOnTrack
    TrkParameters
    TrkFitterInterfaces
    TrkFitterUtils
    AtlasDetDescr
    IdDictDetDescr
    EventPrimitives
    TrkDetDescrInterfaces
    TrkGeometry
    TrkSurfaces
    TrkCompetingRIOsOnTrack
    TrkMeasurementBase
    TrkPrepRawData
    TrkPseudoMeasurementOnTrack
    TrkRIO_OnTrack
    TrkSegment
    TrkTrack
    TrkVertexOnTrack
    TrkExInterfaces
    TrkExUtils
    TrkToolInterfaces
    MagFieldElements
    MagFieldConditions
)

# Component(s) in the package:
atlas_add_component(
    TrkGlobalChi2Fitter
    src/*.cxx
    src/components/*.cxx
    LINK_LIBRARIES
    TrkGlobalChi2FitterLib
)
