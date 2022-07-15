# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from IOVDbSvc.IOVDbSvcConfig import addFoldersSplitOnline
from SubDetectorEnvelopes.SubDetectorEnvelopesConfigNew import EnvelopeDefSvcCfg

# This file is a placeholder - the entire way we build geometry needs to be rewritten so this is to unblock new configuration developments for the moment.
# It is based on: https://gitlab.cern.ch/atlas/athena/blob/master/Tracking/TrkDetDescr/TrkDetDescrSvc/python/AtlasTrackingGeometrySvc.py#L112

def _setupCondDB(flags, CoolDataBaseFolder, quiet=True):
    result=ComponentAccumulator()

    # the tag names
    materialTagBase = 'AtlasLayerMat_v'
    version = 21
    sub_version = ''

    AtlasMaterialTag = materialTagBase+str(version)+sub_version+'_'
    if flags.Detector.GeometryITk:
        AtlasMaterialTag = flags.ITk.trackingGeometry.materialTag+str(flags.ITk.trackingGeometry.version)+'_'
    cfolder = CoolDataBaseFolder +'<tag>TagInfoMajor/'+AtlasMaterialTag+'/GeoAtlas</tag>'

    if flags.Detector.GeometryITk and flags.ITk.trackingGeometry.loadLocalDbForMaterialMaps:
        DataBaseName=flags.ITk.trackingGeometry.localDatabaseName
        from IOVDbSvc.IOVDbSvcConfig import addFolders
        result.merge(addFolders(flags,"/GLOBAL/TrackingGeo/LayerMaterialITK",detDb=DataBaseName, tag=AtlasMaterialTag))
        cfolder = CoolDataBaseFolder +'<tag>TagInfoMajor/'+AtlasMaterialTag+'</tag>'

    # if not quiet:
    #   print('[ TrackingGeometrySvc ]     base material tag : ' + AtlasMaterialTag)
    #   print('[ TrackingGeometrySvc ]     translated to COOL: ' + cfolder)

    # load the right folders
    result.merge( addFoldersSplitOnline(flags,'GLOBAL',[cfolder],[cfolder],splitMC=True) )

    return result


def BeamPipeBuilderCfg(flags, namePrefix=''):
    result = ComponentAccumulator()
    from BeamPipeGeoModel.BeamPipeGMConfig import BeamPipeGeometryCfg
    result.merge(BeamPipeGeometryCfg(flags))
    result.setPrivateTools(CompFactory.InDet.BeamPipeBuilder(name=namePrefix+'BeamPipeBuilder'))
    return result


def ITkBeamPipeBuilderCfg(flags, namePrefix=''):
    result = ComponentAccumulator()
    from BeamPipeGeoModel.BeamPipeGMConfig import BeamPipeGeometryCfg
    result.merge(BeamPipeGeometryCfg(flags))
    result.setPrivateTools(CompFactory.InDet.BeamPipeBuilder(name=namePrefix+'BeamPipeBuilder',
                                                             BeamPipeMaterialBinsZ = flags.ITk.trackingGeometry.beampipeMatZbins))
    return result


def ITkBeamPipeProviderCfg(flags, namePrefix=''):
    result = ComponentAccumulator()
    BeamPipeBuilder = result.popToolsAndMerge(ITkBeamPipeBuilderCfg(flags, namePrefix))
    # add it to tool service
    result.addPublicTool(BeamPipeBuilder)

    result.setPrivateTools(CompFactory.Trk.LayerProvider(name=namePrefix+'BeamPipeProviderInner',
                                                         LayerBuilder = BeamPipeBuilder))
    return result


def PixelLayerBuilderCfg(flags, namePrefix='', setLayerAssociation = True):
    result = ComponentAccumulator()
    from AthenaConfiguration.Enums import ProductionStep
    if flags.Common.ProductionStep != ProductionStep.Simulation: # Not needed by FATRAS
        from PixelGeoModel.PixelGeoModelConfig import PixelReadoutGeometryCfg
        result.merge(PixelReadoutGeometryCfg(flags))
    else:
        from PixelGeoModel.PixelGeoModelConfig import PixelSimulationGeometryCfg
        result.merge(PixelSimulationGeometryCfg(flags))

    result.setPrivateTools(CompFactory.InDet.SiLayerBuilder(name=namePrefix+'PixelLayerBuilder',
                                                            PixelCase=True,
                                                            Identification='Pixel',
                                                            SiDetManagerLocation='Pixel',
                                                            # additional layers - handle with care !
                                                            BarrelAdditionalLayerRadii=[ 130.0 ],   # The PST
                                                            BarrelAdditionalLayerType=[ 0 ],     # -- will shift volume boundary to PST
                                                            EndcapAdditionalLayerPositionsZ=[ -1900. , 1900. ], # DBM
                                                            EndcapAdditionalLayerType=[  1 , 1 ], # DBM
                                                            # Pixel barrel specifications
                                                            BarrelLayerBinsZ=1,
                                                            BarrelLayerBinsPhi=1,
                                                            EndcapLayerBinsR=1,
                                                            EndcapLayerBinsPhi=1,
                                                            # set the layer association
                                                            SetLayerAssociation=setLayerAssociation))
    return result


def ITkPixelLayerBuilderInnerCfg(flags, namePrefix='', setLayerAssociation = True):
    result = ComponentAccumulator()
    from AthenaConfiguration.Enums import ProductionStep
    if flags.Common.ProductionStep != ProductionStep.Simulation: # Not needed by FATRAS
        from PixelGeoModelXml.ITkPixelGeoModelConfig import ITkPixelReadoutGeometryCfg
        result.merge(ITkPixelReadoutGeometryCfg(flags))
    else:
        from PixelGeoModelXml.ITkPixelGeoModelConfig import ITkPixelSimulationGeometryCfg
        result.merge(ITkPixelSimulationGeometryCfg(flags))

    result.setPrivateTools(CompFactory.InDet.SiLayerBuilder(name=namePrefix+'PixelLayerBuilderInner',
                                                            PixelCase            = True,
                                                            Identification       = 'ITkPixelInner',
                                                            SiDetManagerLocation = 'ITkPixel',
                                                            LayerIndicesBarrel   = [0,1],
                                                            LayerIndicesEndcap   = [0,1,2],
                                                            UseRingLayout        = True,
                                                            # Pixel barrel specifications
                                                            BarrelLayerBinsZ     = flags.ITk.trackingGeometry.pixelBarrelMatZbins,
                                                            BarrelLayerBinsPhi   = flags.ITk.trackingGeometry.pixelBarrelMatPhiBins,
                                                            EndcapLayerBinsR     = flags.ITk.trackingGeometry.pixelEndcapMatRbins,
                                                            EndcapLayerBinsPhi   = flags.ITk.trackingGeometry.pixelEndcapMatPhiBins,
                                                            # set the layer association
                                                            SetLayerAssociation  = setLayerAssociation))
    return result


def ITkPixelLayerProviderInnerCfg(flags, namePrefix='', setLayerAssociation = True):
    result = ComponentAccumulator()
    PixelLayerBuilderInner = result.popToolsAndMerge(ITkPixelLayerBuilderInnerCfg(flags, namePrefix, setLayerAssociation))
    # add it to tool service
    result.addPublicTool(PixelLayerBuilderInner)

    result.setPrivateTools(CompFactory.Trk.LayerProvider(name=namePrefix+'PixelProviderInner',
                                                         LayerBuilder = PixelLayerBuilderInner))
    return result


def ITkPixelLayerBuilderOuterCfg(flags, namePrefix='', setLayerAssociation = True):
    result = ComponentAccumulator()
    from AthenaConfiguration.Enums import ProductionStep
    if flags.Common.ProductionStep != ProductionStep.Simulation: # Not needed by FATRAS
        from PixelGeoModelXml.ITkPixelGeoModelConfig import ITkPixelReadoutGeometryCfg
        result.merge(ITkPixelReadoutGeometryCfg(flags))
    else:
        from PixelGeoModelXml.ITkPixelGeoModelConfig import ITkPixelSimulationGeometryCfg
        result.merge(ITkPixelSimulationGeometryCfg(flags))

    result.setPrivateTools(CompFactory.InDet.SiLayerBuilder(name=namePrefix+'PixelLayerBuilderOuter',
                                                            PixelCase            = True,
                                                            Identification       = 'ITkPixelOuter',
                                                            SiDetManagerLocation = 'ITkPixel',
                                                            LayerIndicesBarrel   = [2,3,4],
                                                            LayerIndicesEndcap   = [3,4,5,6,7,8],
                                                            UseRingLayout=True,
                                                            BarrelLayerBinsZ     = flags.ITk.trackingGeometry.pixelBarrelMatZbins,
                                                            BarrelLayerBinsPhi   = flags.ITk.trackingGeometry.pixelBarrelMatPhiBins,
                                                            EndcapLayerBinsR     = flags.ITk.trackingGeometry.pixelEndcapMatRbins,
                                                            EndcapLayerBinsPhi   = flags.ITk.trackingGeometry.pixelEndcapMatPhiBins,
                                                            # set the layer association
                                                            SetLayerAssociation  = setLayerAssociation))
    return result


def ITkPixelLayerProviderOuterCfg(flags, namePrefix='', setLayerAssociation = True):
    result = ComponentAccumulator()
    PixelLayerBuilderOuter = result.popToolsAndMerge(ITkPixelLayerBuilderOuterCfg(flags, namePrefix, setLayerAssociation))
    # add it to tool service
    result.addPublicTool(PixelLayerBuilderOuter)

    result.setPrivateTools(CompFactory.Trk.LayerProvider(name=namePrefix+'PixelProviderOuter',
                                                         LayerBuilder = PixelLayerBuilderOuter))
    return result


def SCT_LayerBuilderCfg(flags, namePrefix='', setLayerAssociation = True):
    result = ComponentAccumulator()
    from AthenaConfiguration.Enums import ProductionStep
    if flags.Common.ProductionStep != ProductionStep.Simulation: # Not needed by FATRAS
        from SCT_GeoModel.SCT_GeoModelConfig import SCT_ReadoutGeometryCfg
        result.merge(SCT_ReadoutGeometryCfg( flags ))
    else:
        from SCT_GeoModel.SCT_GeoModelConfig import SCT_SimulationGeometryCfg
        result.merge(SCT_SimulationGeometryCfg( flags ))

    result.setPrivateTools(CompFactory.InDet.SiLayerBuilder(name=namePrefix+'SCT_LayerBuilder',
                                                            PixelCase=False,
                                                            Identification='SCT',
                                                            SiDetManagerLocation='SCT',
                                                            # additionall layers - handle with care !
                                                            BarrelAdditionalLayerRadii=[],
                                                            BarrelAdditionalLayerType=[],
                                                            EndcapAdditionalLayerPositionsZ=[ -2850. , 2850. ],
                                                            EndcapAdditionalLayerType=[  0 , 0 ],
                                                            BarrelLayerBinsZ=1,
                                                            BarrelLayerBinsPhi=1,
                                                            # SCT endcap specifications
                                                            EndcapLayerBinsR=1,
                                                            EndcapLayerBinsPhi=1,
                                                            EndcapComplexRingBinning=True,
                                                            # set the layer association
                                                            SetLayerAssociation=setLayerAssociation))
    return result


def ITkStripLayerBuilderCfg(flags, namePrefix='', setLayerAssociation = True):
    result = ComponentAccumulator()
    from AthenaConfiguration.Enums import ProductionStep
    if flags.Common.ProductionStep != ProductionStep.Simulation: # Not needed by FATRAS
        from StripGeoModelXml.ITkStripGeoModelConfig import ITkStripReadoutGeometryCfg
        result.merge(ITkStripReadoutGeometryCfg(flags))
    else:
        from StripGeoModelXml.ITkStripGeoModelConfig import ITkStripSimulationGeometryCfg
        result.merge(ITkStripSimulationGeometryCfg(flags))

    result.setPrivateTools(CompFactory.InDet.SiLayerBuilder(name=namePrefix+'SCT_LayerBuilder',
                                                            PixelCase                       = False,
                                                            Identification                  = 'ITkStrip',
                                                            SiDetManagerLocation            = 'ITkStrip',
                                                            AddMoreSurfaces                 = True,
                                                            BarrelLayerBinsZ                = flags.ITk.trackingGeometry.stripBarrelMatZbins,
                                                            BarrelLayerBinsPhi              = flags.ITk.trackingGeometry.stripBarrelMatPhiBins,
                                                            EndcapLayerBinsR                = flags.ITk.trackingGeometry.stripEndcapMatRbins,
                                                            EndcapLayerBinsPhi              = flags.ITk.trackingGeometry.stripEndcapMatPhiBins,
                                                            # set the layer association
                                                            SetLayerAssociation             = setLayerAssociation))
    return result


def ITkStripLayerProviderCfg(flags, namePrefix='', setLayerAssociation = True):
    result = ComponentAccumulator()
    ITkStripBuilder = result.popToolsAndMerge(ITkStripLayerBuilderCfg(flags, namePrefix, setLayerAssociation))
    # add it to tool service
    result.addPublicTool(ITkStripBuilder)

    result.setPrivateTools(CompFactory.Trk.LayerProvider(name=namePrefix+'StripProvider',
                                                         LayerBuilder = ITkStripBuilder))
    return result


def TRT_LayerBuilderCfg(flags, namePrefix='', buildTrtStrawLayers = False):
    result = ComponentAccumulator()
    from AthenaConfiguration.Enums import ProductionStep
    if flags.Common.ProductionStep != ProductionStep.Simulation: # Not needed by FATRAS
        from TRT_GeoModel.TRT_GeoModelConfig import TRT_ReadoutGeometryCfg
        result.merge(TRT_ReadoutGeometryCfg( flags ))
    else:
        from TRT_GeoModel.TRT_GeoModelConfig import TRT_SimulationGeometryCfg
        result.merge(TRT_SimulationGeometryCfg( flags ))

    TRT_LayerBuilder = CompFactory.InDet.TRT_LayerBuilder(name=namePrefix+'TRT_LayerBuilder')
    # TRT barrel specifications - assume defaults
    # TRT endcap specifications - assume defaults

    from SimulationConfig.SimEnums import SimulationFlavour
    if buildTrtStrawLayers or (flags.Common.ProductionStep in [ProductionStep.Simulation, ProductionStep.FastChain] and flags.Sim.ISF.Simulator not in [SimulationFlavour.ATLFASTIIMT]):
        TRT_LayerBuilder.ModelLayersOnly = False
    result.setPrivateTools(TRT_LayerBuilder)
    return result


def InDetTrackingGeometryBuilderCfg(name, flags, namePrefix='', setLayerAssociation = True):
    # Based on https://gitlab.cern.ch/atlas/athena/blob/master/InnerDetector/InDetDetDescr/InDetTrackingGeometry/python/ConfiguredInDetTrackingGeometryBuilder.py
    # A lot of comments below are to help people understand differences from the above, in case we need to revert some simplifications I made
    # i.e. this is far from complete, but is better than what was there before.

    result = ComponentAccumulator()

    # beampipe
    beamPipeBuilder = result.popToolsAndMerge(BeamPipeBuilderCfg(flags, namePrefix))
    result.addPublicTool(beamPipeBuilder)

    layerbuilders = [] # Used to set a PublicToolHandleArray
    binnings = []
    colors = []

    if flags.Detector.GeometryPixel:
        PixelLayerBuilder = result.popToolsAndMerge(PixelLayerBuilderCfg(flags, namePrefix, setLayerAssociation))
        # add it to tool service
        result.addPublicTool(PixelLayerBuilder)
        # put them to the caches
        layerbuilders += [ result.getPublicTool(PixelLayerBuilder.name) ]
        binnings += [ 2 ] # PixelLayerBinning - the binning type of the layers
        colors += [ 3 ]

    if flags.Detector.GeometrySCT:
        SCT_LayerBuilder = result.popToolsAndMerge(SCT_LayerBuilderCfg(flags, namePrefix, setLayerAssociation))
        # add it to tool service
        result.addPublicTool(SCT_LayerBuilder)
        # put them to the caches
        layerbuilders += [ result.getPublicTool(SCT_LayerBuilder.name) ]
        binnings += [ 2 ] # SCT_LayerBinning - the binning type of the layer
        colors += [ 4 ]

    if flags.Detector.GeometryTRT:
        from AthenaConfiguration.Enums import BeamType
        buildTrtStrawLayers = (flags.Beam.Type == BeamType.Cosmics)
        TRT_LayerBuilder = result.popToolsAndMerge(TRT_LayerBuilderCfg(flags, namePrefix, buildTrtStrawLayers))
        # add it to tool service
        result.addPublicTool(TRT_LayerBuilder)
        # put them to the caches
        layerbuilders += [ result.getPublicTool(TRT_LayerBuilder.name) ]
        # set the binning from bi-aequidistant to arbitrary for complex TRT volumes
        TRT_LayerBinning = 1
        from AthenaConfiguration.Enums import ProductionStep
        from SimulationConfig.SimEnums import SimulationFlavour
        if buildTrtStrawLayers or (flags.Common.ProductionStep in [ProductionStep.Simulation, ProductionStep.FastChain] and flags.Sim.ISF.Simulator not in [SimulationFlavour.ATLFASTIIMT]):
            TRT_LayerBinning = 2
        binnings += [ TRT_LayerBinning ]
        colors += [ 5 ]

    # helpers for the InDetTrackingGeometry Builder : layer array creator
    from TrkConfig.TrkDetDescrToolsConfig import InDetLayerArrayCreatorCfg, InDetTrackingVolumeArrayCreatorCfg, InDetTrackingVolumeHelperCfg, InDetCylinderVolumeCreatorCfg
    InDetLayerArrayCreator = result.popToolsAndMerge(InDetLayerArrayCreatorCfg(flags))
    # add to ToolSvc
    result.addPublicTool(InDetLayerArrayCreator)

    # helpers for the InDetTrackingGeometry Builder : volume array creator
    InDetTrackingVolumeArrayCreator = result.popToolsAndMerge(InDetTrackingVolumeArrayCreatorCfg(flags))
    # add to ToolSvc
    result.addPublicTool(InDetTrackingVolumeArrayCreator)

    # helpers for the InDetTrackingGeometry Builder : tracking volume helper for gluing
    InDetTrackingVolumeHelper = result.popToolsAndMerge(InDetTrackingVolumeHelperCfg(flags))
    # the material bins - assume defaults
    # add to ToolSvc
    result.addPublicTool(InDetTrackingVolumeHelper)

    # helpers for the InDetTrackingGeometry Builder : cylinder volume creator
    InDetCylinderVolumeCreator = result.popToolsAndMerge(InDetCylinderVolumeCreatorCfg(flags))
    # add to ToolSvc
    result.addPublicTool(InDetCylinderVolumeCreator)

    # the tracking geometry builder
    result.setPrivateTools(CompFactory.InDet.RobustTrackingGeometryBuilder(namePrefix+name,
                                                BeamPipeBuilder = result.getPublicTool(beamPipeBuilder.name), # public tool handle
                                                LayerBuilders = layerbuilders, # public tool handle array
                                                LayerBinningType = binnings,
                                                ColorCodes = colors,
                                                EnvelopeDefinitionSvc = result.getPrimaryAndMerge(EnvelopeDefSvcCfg(flags)).name,
                                                VolumeEnclosureDiscPositions = [ 3000., 3450. ],
                                                TrackingVolumeCreator = result.getPublicTool(InDetCylinderVolumeCreator.name), # public tool handle
                                                LayerArrayCreator = result.getPublicTool(InDetLayerArrayCreator.name), # public tool handle
                                                ReplaceAllJointBoundaries = True,
                                                VolumeEnclosureCylinderRadii = [],
                                                BuildBoundaryLayers = True,
                                                ExitVolumeName = 'InDet::Containers::InnerDetector')

                                                # FIXME - not sure what to do about the following:
                                                # BuildBoundaryLayers       = TrkDetFlags.InDetBuildMaterialBoundaries(),
                                                # ReplaceAllJointBoundaries = TrkDetFlags.InDetBuildJointBoundaries(),
                                                # OutputLevel               = TrkDetFlags.InDetBuildingOutputLevel(),
                                                # ExitVolumeName            = TrkDetFlags.InDetContainerName(),

                                                # Probably they should just be dropped, but I leave this comment for the moment so reviewers can have a think as well.

                                                # Barrel Entry layers (in old config) etc were removed in 323990adfce581a635ae1809fd2ecc6a093a704c (!)
                       )
    return result


def ITkTrackingGeometryBuilderCfg(name, flags, namePrefix='', setLayerAssociation = True):
    # Based on https://gitlab.cern.ch/atlas/athena/blob/master/InnerDetector/InDetDetDescr/InDetTrackingGeometry/python/ConfiguredInDetTrackingGeometryBuilder.py
    # A lot of comments below are to help people understand differences from the above, in case we need to revert some simplifications I made
    # i.e. this is far from complete, but is better than what was there before.
    result = ComponentAccumulator()
    # beampipe
    beamPipeProvider = result.popToolsAndMerge(ITkBeamPipeProviderCfg(flags, namePrefix))
    # add it to tool service
    result.addPublicTool(beamPipeProvider)
    BeamPipeBinning = 2
    layerProviders = [beamPipeProvider]
    binnings_barrel = [BeamPipeBinning]
    binnings_endcap = [BeamPipeBinning]
    colors          = [2]

    # Pixel
    if flags.Detector.GeometryITkPixel:
        pixelProviderInner = result.popToolsAndMerge(ITkPixelLayerProviderInnerCfg(flags, namePrefix, setLayerAssociation))
        # add it to tool service
        result.addPublicTool(pixelProviderInner)
        # the binning type of the layers   a
        PixelLayerBinning = 2
        # put them to the caches
        layerProviders  += [pixelProviderInner]
        binnings_barrel += [ PixelLayerBinning ]
        binnings_endcap += [ PixelLayerBinning ]
        colors          += [ 3 ]

        pixelProviderOuter = result.popToolsAndMerge(ITkPixelLayerProviderOuterCfg(flags, namePrefix, setLayerAssociation))
        # add it to tool service
        result.addPublicTool(pixelProviderOuter)
        # the binning type of the layers   a
        PixelLayerBinning = 2
        # put them to the caches
        layerProviders  += [pixelProviderOuter]
        binnings_barrel += [ PixelLayerBinning ]
        binnings_endcap += [ PixelLayerBinning ]
        colors          += [ 3 ]

    if flags.Detector.GeometryITkStrip:
        stripProvider = result.popToolsAndMerge(ITkStripLayerProviderCfg(flags, namePrefix, setLayerAssociation))
        # add it to tool service
        result.addPublicTool(stripProvider)
        # the binning type of the layer
        SCT_LayerBinning = 2

        # put them to the caches
        layerProviders  += [stripProvider]
        binnings_barrel += [ SCT_LayerBinning ]
        binnings_endcap += [ SCT_LayerBinning ]
        colors          += [ 4 ]

    from TrkConfig.TrkDetDescrToolsConfig import InDetLayerArrayCreatorCfg, InDetTrackingVolumeArrayCreatorCfg, ITkTrackingVolumeHelperCfg, ITkCylinderVolumeCreatorCfg
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
    InDetCylinderVolumeCreator = result.popToolsAndMerge(ITkCylinderVolumeCreatorCfg(flags))
    # add to ToolSvc
    result.addPublicTool(InDetCylinderVolumeCreator)

    # the tracking geometry builder
    result.setPrivateTools(CompFactory.InDet.StagedTrackingGeometryBuilder(namePrefix+name,
                                                                           LayerBuilders     = layerProviders,
                                                                           LayerBinningTypeCenter    = binnings_barrel,
                                                                           LayerBinningTypeEndcap    = binnings_endcap,
                                                                           ColorCodes                = colors,
                                                                           EnvelopeDefinitionSvc     = result.getPrimaryAndMerge(EnvelopeDefSvcCfg(flags)).name,
                                                                           TrackingVolumeCreator     = InDetCylinderVolumeCreator,
                                                                           LayerArrayCreator         = InDetLayerArrayCreator,
                                                                           CheckForRingLayout        = True,
                                                                           MinimalRadialGapForVolumeSplit = flags.ITk.trackingGeometry.minimalRadialGapForVolumeSplit,
                                                                           ReplaceAllJointBoundaries = True,
                                                                           BuildBoundaryLayers=True,
                                                                           ExitVolumeName='InDet::Containers::InnerDetector'))
    return result


# Replaces https://gitlab.cern.ch/atlas/athena/blob/master/Calorimeter/CaloTrackingGeometry/python/ConfiguredCaloTrackingGeometryBuilder.py
def CaloTrackingGeometryBuilderCfg(name, flags, namePrefix=''):
    result = ComponentAccumulator()
    # The following replaces LArCalorimeter/LArTrackingGeometry/python/ConfiguredLArVolumeBuilder.py
    from LArGeoAlgsNV.LArGMConfig import LArGMCfg
    result.merge(LArGMCfg(flags))
    from TrkConfig.TrkDetDescrToolsConfig import TrackingVolumeHelperCfg
    trackingVolumeHelper = result.popToolsAndMerge(TrackingVolumeHelperCfg(flags))
    result.addPublicTool(trackingVolumeHelper)

    lArVolumeBuilder = CompFactory.LAr.LArVolumeBuilder(name="LArVolumeBuilder",
                                                        BarrelEnvelopeCover = 5.0,
                                                        EndcapEnvelopeCover = 5.0,
                                                        TrackingVolumeHelper = result.getPublicTool(trackingVolumeHelper.name))
    result.addPublicTool(lArVolumeBuilder)

    # The following replaces TileCalorimeter/TileTrackingGeometry/python/ConfiguredTileVolumeBuilder.py
    from TileGeoModel.TileGMConfig import TileGMCfg
    result.merge(TileGMCfg(flags))
    tileVolumeBuilder = CompFactory.Tile.TileVolumeBuilder(name="TileVolumeBuilder",
                                                           TrackingVolumeHelper = result.getPublicTool(trackingVolumeHelper.name))
    result.addPublicTool(tileVolumeBuilder)

    caloVolumeCreator = CompFactory.Trk.CylinderVolumeCreator("CaloVolumeCreator")
    result.addPublicTool(caloVolumeCreator)

    result.setPrivateTools(CompFactory.Calo.CaloTrackingGeometryBuilder(namePrefix+name,
                                                                        LArVolumeBuilder = result.getPublicTool(lArVolumeBuilder.name), # public tool handle
                                                                        TileVolumeBuilder = result.getPublicTool(tileVolumeBuilder.name), # public tool handle
                                                                        TrackingVolumeHelper = result.getPublicTool(trackingVolumeHelper.name), # public tool handle
                                                                        EnvelopeDefinitionSvc = result.getPrimaryAndMerge(EnvelopeDefSvcCfg(flags)).name,
                                                                        EntryVolumeName = "InDet::Containers::EntryVolume",
                                                                        ExitVolumeName = "Calo::Container",
                                                                        GapLayerEnvelope=5.0,
                                                                        TrackingVolumeCreator = result.getPublicTool(caloVolumeCreator.name) # public tool handle
                                                         ))
    return result


def MuonTrackingGeometryBuilderCfg(flags):
    result = ComponentAccumulator()
    # Copied from from MuonTrackingGeometry.ConfiguredMuonTrackingGeometry import MuonTrackingGeometryBuilder
    # Add the muon geometry model to the CA
    from MuonConfig.MuonGeometryConfig import MuonGeoModelCfg
    result.merge(MuonGeoModelCfg(flags))
    muonStationTypeBuilder = CompFactory.Muon.MuonStationTypeBuilder(name='MuonStationTypeBuilder')

    muonStationBuilder = CompFactory.Muon.MuonStationBuilder(name='MuonStationBuilder')
    muonStationBuilder.StationTypeBuilder = muonStationTypeBuilder

    muonInertMaterialBuilder = CompFactory.Muon.MuonInertMaterialBuilder(name='MuonInertMaterialBuilder')

    muonTrackingGeometryBuilder = CompFactory.Muon.MuonTrackingGeometryBuilder(name='MuonTrackingGeometryBuilder',
                                                                               EnvelopeDefinitionSvc=result.getPrimaryAndMerge(EnvelopeDefSvcCfg(flags)).name,
                                                                               MuonStationBuilder = muonStationBuilder,
                                                                               InertMaterialBuilder=muonInertMaterialBuilder
    )
    # import pdb ; pdb.set_trace()
    muonTrackingGeometryBuilder.EntryVolumeName = 'MuonSpectrometerEntrance'
    muonTrackingGeometryBuilder.ExitVolumeName  = 'Muon::Containers::MuonSystem'
    result.setPrivateTools(muonTrackingGeometryBuilder)
    return result


# Originally this function would use was TrkDetFlags.MaterialSource() and TrkDetFlags.MaterialValidation(). For new configuration, (temporarily?) pass as parameters.
# https://gitlab.cern.ch/atlas/athena/blob/master/Tracking/TrkDetDescr/TrkDetDescrSvc/python/AtlasTrackingGeometrySvc.py#L112
def TrackingGeometrySvcCfg( flags , name = 'AtlasTrackingGeometrySvc', doMaterialValidation=False ) :
    """
    Sets up the Tracking Geometry Service
    """
    result = ComponentAccumulator()
    atlas_tracking_geometry_name = 'AtlasTrackingGeometry'
    from TrkConfig.TrkDetDescrToolsConfig import TrackingVolumeHelperCfg, TrackingVolumeArrayCreatorCfg
    # helpers for the TrackingGeometry Builder : volume array creator
    TrackingVolumeArrayCreator = result.popToolsAndMerge(TrackingVolumeArrayCreatorCfg(flags))
    # add to ToolSvc
    result.addPublicTool(TrackingVolumeArrayCreator)

    # helpers for the TrackingGeometry Builder : tracking volume helper for gluing
    TrackingVolumeHelper = result.popToolsAndMerge(TrackingVolumeHelperCfg(flags))
    # the material bins - assume defaults
    # add to ToolSvc
    result.addPublicTool(TrackingVolumeHelper)

    atlas_geometry_builder = CompFactory.Trk.GeometryBuilder(name='AtlasGeometryBuilder',
                                                             WorldDimension=[],
                                                             WorldMaterialProperties=[],
                                                             TrackingVolumeArrayCreator=result.getPublicTool(TrackingVolumeArrayCreator.name),
                                                             TrackingVolumeHelper=result.getPublicTool(TrackingVolumeHelper.name))

    # Depending on the job configuration, setup the various detector builders, and add to atlas_geometry_builder
    if flags.Detector.GeometryID:
        # TODO Not sure how to handle TrkDetFlags, specifically ISF_FatrasCustomGeometry, XMLFastCustomGeometry
        # So, here we only setup the default InDet geometry builder!
        atlas_geometry_builder.InDetTrackingGeometryBuilder = result.popToolsAndMerge(
            InDetTrackingGeometryBuilderCfg(name='InDetTrackingGeometryBuilder', flags=flags)
        )

    elif flags.Detector.GeometryITk:
        atlas_geometry_builder.InDetTrackingGeometryBuilder = result.popToolsAndMerge(
            ITkTrackingGeometryBuilderCfg(name ='InDetTrackingGeometryBuilder', flags=flags)
        )

    if flags.Detector.GeometryCalo:
        atlas_geometry_builder.CaloTrackingGeometryBuilder = result.popToolsAndMerge(
            CaloTrackingGeometryBuilderCfg(name='CaloTrackingGeometryBuilder', flags = flags)
        )

    if flags.Detector.GeometryMuon:
        atlas_geometry_builder.MuonTrackingGeometryBuilder = result.popToolsAndMerge(
            MuonTrackingGeometryBuilderCfg(flags)
        )

    # Now set up processors
    atlas_geometry_processors=[] # array of private ToolHandles

    if flags.TrackingGeometry.MaterialSource == 'COOL':

        CoolDataBaseFolder = '/GLOBAL/TrackingGeo/LayerMaterialV2'
        if flags.Detector.GeometryITk:
            CoolDataBaseFolder = '/GLOBAL/TrackingGeo/LayerMaterialITK'

        # the material provider
        atlasMaterialProvider = CompFactory.Trk.LayerMaterialProvider('AtlasMaterialProvider', LayerMaterialMapName=CoolDataBaseFolder, LayerMaterialMapKey='')
        atlas_geometry_processors += [ atlasMaterialProvider ]

        # Setup DBs
        result.merge(_setupCondDB(flags, CoolDataBaseFolder))

    elif  flags.TrackingGeometry.MaterialSource == 'Input':
        atlasMaterialProvider = CompFactory.Trk.InputLayerMaterialProvider('AtlasMaterialProvider')
        atlas_geometry_processors += [ atlasMaterialProvider ]

    if doMaterialValidation:
        atlasLayerMaterialInspector = CompFactory.Trk.LayerMaterialInspector('AtlasLayerMaterialInspector')
        atlas_geometry_processors += [ atlasLayerMaterialInspector ]

    from GaudiKernel.GaudiHandles import PrivateToolHandleArray
    result.addService(CompFactory.Trk.TrackingGeometrySvc( name, GeometryBuilder = atlas_geometry_builder,
                                    TrackingGeometryName = atlas_tracking_geometry_name,
                                    GeometryProcessors = PrivateToolHandleArray(atlas_geometry_processors),
                                    BuildGeometryFromTagInfo = True), primary = True)

    return result

if __name__ == '__main__':
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    from AthenaConfiguration.TestDefaults import defaultTestFiles

    ConfigFlags.Input.Files = defaultTestFiles.RAW
    ConfigFlags.lock()

    acc = TrackingGeometrySvcCfg(ConfigFlags )

    f=open('TrackingGeometrySvcCfg.pkl','wb')
    acc.store(f)
    f.close()
