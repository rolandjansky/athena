# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

def ActsTrkPixelSpacePointToolCfg(flags, name = "ActsTrkPixelSpacePointTool", **kwargs):
    acc = ComponentAccumulator()

    acc.setPrivateTools(CompFactory.ActsTrk.PixelSpacePointFormationTool(name, **kwargs))
    return acc

def ActsTrkStripSpacePointToolCfg(flags, name = "ActsTrkStripSpacePointTool", **kwargs):
    acc = ComponentAccumulator()

    from SiLorentzAngleTool.ITkStripLorentzAngleConfig import ITkStripLorentzAngleToolCfg
    kwargs.setdefault("LorentzAngleTool", acc.popToolsAndMerge(ITkStripLorentzAngleToolCfg(flags)) )

    kwargs.setdefault("AllClusters", False)

    acc.setPrivateTools(CompFactory.ActsTrk.StripSpacePointFormationTool(name, **kwargs))
    return acc

def ActsTrkPixelSpacePointFormationCfg(ConfigFlags,
                                       name = "ActsTrkPixelSpacePointFormation",
                                       **kwargs):

    from PixelGeoModelXml.ITkPixelGeoModelConfig import ITkPixelReadoutGeometryCfg
    acc = ITkPixelReadoutGeometryCfg(ConfigFlags)

    ActsTrkPixelSpacePointTool = acc.popToolsAndMerge(ActsTrkPixelSpacePointToolCfg(ConfigFlags))
    kwargs.setdefault("SpacePointFormationTool", ActsTrkPixelSpacePointTool)

    kwargs.setdefault("PixelClusters", "ITkPixelClusters")
    kwargs.setdefault("PixelDetectorElements", "ITkPixelDetectorElementCollection")

    kwargs.setdefault("PixelSpacePoints", "ITkPixelSpacePoints")
    kwargs.setdefault("PixelSpacePointData", "ITkPixelSpacePointData")

    acc.addEventAlgo(CompFactory.ActsTrk.PixelSpacePointFormationAlgorithm(name, **kwargs))
    return acc

def ActsTrkStripSpacePointFormationCfg(ConfigFlags,
                                       name = "ActsTrkStripSpacePointFormation",
                                       **kwargs):

    from StripGeoModelXml.ITkStripGeoModelConfig import ITkStripReadoutGeometryCfg
    acc = ITkStripReadoutGeometryCfg(ConfigFlags)

    ActsTrkStripSpacePointTool = acc.popToolsAndMerge(ActsTrkStripSpacePointToolCfg(ConfigFlags))
    kwargs.setdefault("SpacePointFormationTool", ActsTrkStripSpacePointTool)

    kwargs.setdefault("StripClusters", "ITkStripClusters")
    kwargs.setdefault("StripDetectorElements", "ITkStripDetectorElementCollection")
    kwargs.setdefault("StripElementPropertiesTable", "ITkStripElementPropertiesTable")

    kwargs.setdefault("StripSpacePoints", "ITkStripSpacePoints")
    kwargs.setdefault("StripSpacePointData", "ITkStripSpacePointData")
    kwargs.setdefault("StripOverlapSpacePoints", "ITkStripOverlapSpacePoints")
    kwargs.setdefault("StripOverlapSpacePointData", "ITkStripOverlapSpacePointData")
    kwargs.setdefault("ProcessOverlapForStrip", True)

    acc.addEventAlgo(CompFactory.ActsTrk.StripSpacePointFormationAlgorithm(name, **kwargs))
    return acc
