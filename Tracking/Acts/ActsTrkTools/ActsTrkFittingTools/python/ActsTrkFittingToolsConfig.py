#  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

from ActsGeometry.ActsGeometryConfig import (
    ActsExtrapolationToolCfg,
    ActsTrackingGeometryToolCfg,
    ActsATLASConverterToolCfg,
)
from TrkConfig.TrkTrackSummaryToolConfig import InDetTrackSummaryToolCfg


def ActsKalmanFitterCfg(flags, name: str = "ActsKalmanFitter", **kwargs):
    result = ComponentAccumulator()

    # Make sure this is set correctly!
    #  /eos/project-a/acts/public/MaterialMaps/ATLAS/material-maps-Pixel-SCT.json

    if "TrackingGeometryTool" not in kwargs:
        trkGeoTool = result.getPrimaryAndMerge(ActsTrackingGeometryToolCfg(flags))
        kwargs["TrackingGeometryTool"] = trkGeoTool

    if "ExtrapolationTool" not in kwargs:
        kwargs["ExtrapolationTool"] = result.getPrimaryAndMerge(
            ActsExtrapolationToolCfg(flags, MaxSteps=10000)
        )

    result.merge(ActsExtrapolationToolCfg(flags, MaxSteps=10000))

    kwargs.setdefault("ReverseFilteringPt", 1.0)  # @TODO: Unit

    ActsATLASConverterTool = result.popToolsAndMerge(ActsATLASConverterToolCfg(flags))
    kwargs["ATLASConverterTool"] = ActsATLASConverterTool

    if "SummaryTool" not in kwargs:
        summary = result.getPrimaryAndMerge(
            InDetTrackSummaryToolCfg(flags, 
                                     #  doHolesInDet=False
                                     )
        )
        kwargs["SummaryTool"] = summary

    if flags.Detector.GeometryITk:
        from InDetConfig.InDetBoundaryCheckToolConfig import ITkBoundaryCheckToolCfg

        kwargs.setdefault(
            "BoundaryCheckTool", result.popToolsAndMerge(ITkBoundaryCheckToolCfg(flags))
        )
    else:
        from InDetConfig.InDetBoundaryCheckToolConfig import InDetBoundaryCheckToolCfg

        kwargs.setdefault(
            "BoundaryCheckTool",
            result.popToolsAndMerge(InDetBoundaryCheckToolCfg(flags)),
        )

    result.setPrivateTools(CompFactory.ActsKalmanFitter(name, **kwargs))

    return result

