# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.AccumulatorCache import AccumulatorCache


def TrackingVolumeHelperCfg(flags, name='TrackingVolumeHelper', **kwargs):
    result = ComponentAccumulator()
    result.setPrivateTools(CompFactory.Trk.TrackingVolumeHelper(name))
    return result


def InDetTrackingVolumeHelperCfg(flags, name='InDetTrackingVolumeHelper', **kwargs): # tool with non-default name but default properties?!
    # helpers for the InDetTrackingGeometry Builder : tracking volume helper for gluing
    # the material bins - assume defaults
    return TrackingVolumeHelperCfg(flags, name, **kwargs)


def ITkTrackingVolumeHelperCfg(flags, name='InDetTrackingVolumeHelper', **kwargs): # TODO Still called InDetTrackingVolumeHelper - check
    kwargs.setdefault("BarrelLayerBinsZ", flags.ITk.trackingGeometry.passiveBarrelMatZbins)
    kwargs.setdefault("BarrelLayerBinsPhi", flags.ITk.trackingGeometry.passiveBarrelMatPhiBins)
    kwargs.setdefault("EndcapLayerBinsR", flags.ITk.trackingGeometry.passiveEndcapMatRbins)
    kwargs.setdefault("EndcapLayerBinsPhi", flags.ITk.trackingGeometry.passiveEndcapMatPhiBins)
    return InDetTrackingVolumeHelperCfg(flags, name, **kwargs)


def TrackingVolumeArrayCreatorCfg(flags, name="TrackingVolumeArrayCreator", **kwargs):
    result = ComponentAccumulator()
    result.setPrivateTools(CompFactory.Trk.TrackingVolumeArrayCreator(name, **kwargs))
    return result


def InDetTrackingVolumeArrayCreatorCfg(flags, name="InDetTrackingVolumeArrayCreator", **kwargs): # tool with non-default name but default properties?!
    return TrackingVolumeArrayCreatorCfg(flags, name, **kwargs)


@AccumulatorCache
def InDetLayerArrayCreatorCfg(flags, name='InDetLayerArrayCreator'):
    result = ComponentAccumulator()
    # helpers for the InDetTrackingGeometry Builder : layer array creator
    result.setPrivateTools(CompFactory.Trk.LayerArrayCreator(name,
                                                             EmptyLayerMode=2  # deletes empty material layers from arrays
                                                         ))
    return result


def InDetCylinderVolumeCreatorCfg(flags, name='InDetCylinderVolumeCreator', **kwargs):
    result = ComponentAccumulator()
    InDetLayerArrayCreator = result.popToolsAndMerge(InDetLayerArrayCreatorCfg(flags))
    result.addPublicTool(InDetLayerArrayCreator)

    # helpers for the TrackingGeometry Builder : volume array creator
    TrackingVolumeArrayCreator = result.popToolsAndMerge(TrackingVolumeArrayCreatorCfg(flags))
    # add to ToolSvc
    result.addPublicTool(TrackingVolumeArrayCreator)

    # helpers for the TrackingGeometry Builder : tracking volume helper for gluing
    TrackingVolumeHelper = result.popToolsAndMerge(TrackingVolumeHelperCfg(flags))
    # the material bins - assume defaults
    # add to ToolSvc
    result.addPublicTool(TrackingVolumeHelper)

    # helpers for the InDetTrackingGeometry Builder : cylinder volume creator
    result.setPrivateTools(CompFactory.Trk.CylinderVolumeCreator(name,
                                                                 PassiveLayerBinsRZ = 1,
                                                                 LayerArrayCreator = result.getPublicTool(InDetLayerArrayCreator.name), # public tool handle
                                                                 # FIXME The configuration of these two tools is only
                                                                 # consistent with InDetTrackingGeometryBuilder
                                                                 # as the default instances and the "InDet" instances
                                                                 # have the same configuration.
                                                                 TrackingVolumeArrayCreator = result.getPublicTool(TrackingVolumeArrayCreator.name), # public tool handle
                                                                 TrackingVolumeHelper = result.getPublicTool(TrackingVolumeHelper.name) # public tool handle
                                                             ))
    # specifiy the binning, passive layers, entry layers - assume defaults
    return result


def ITkCylinderVolumeCreatorCfg(flags, name='InDetCylinderVolumeCreator', **kwargs):# TODO Still called InDetCylinderVolumeCreator - check
    result = ComponentAccumulator()
    # helpers for the InDetTrackingGeometry Builder : layer array creator
    InDetLayerArrayCreator = result.popToolsAndMerge(InDetLayerArrayCreatorCfg(flags))
    # add to ToolSvc
    result.addPublicTool(InDetLayerArrayCreator)

    # helpers for the InDetTrackingGeometry Builder : volume array creator
    InDetTrackingVolumeArrayCreator = result.popToolsAndMerge(InDetTrackingVolumeArrayCreatorCfg(flags))
    # add to ToolSvc
    result.addPublicTool(InDetTrackingVolumeArrayCreator)

    # helpers for the InDetTrackingGeometry Builder : tracking volume helper for gluing
    InDetTrackingVolumeHelper = result.popToolsAndMerge(ITkTrackingVolumeHelperCfg(flags))
    # the material bins - assume defaults
    # add to ToolSvc
    result.addPublicTool(InDetTrackingVolumeHelper)

    # helpers for the InDetTrackingGeometry Builder : cylinder volume creator
    InDetCylinderVolumeCreator = CompFactory.Trk.CylinderVolumeCreator(name,
                                                                       LayerArrayCreator = result.getPublicTool(InDetLayerArrayCreator.name), # public tool handle
                                                                       TrackingVolumeArrayCreator = result.getPublicTool(InDetTrackingVolumeArrayCreator.name), # public tool handle
                                                                       TrackingVolumeHelper       = result.getPublicTool(InDetTrackingVolumeHelper.name), # public tool handle
                                                                       PassiveLayerThickness      = 1., ## mm
                                                                       PassiveLayerBinsRZ   = flags.ITk.trackingGeometry.passiveBarrelMatZbins,
                                                                       PassiveLayerBinsPhi  = flags.ITk.trackingGeometry.passiveBarrelMatPhiBins)
    # specifiy the binning, passive layers, entry layers - assume defaults
    result.setPrivateTools(InDetCylinderVolumeCreator)
    return result
