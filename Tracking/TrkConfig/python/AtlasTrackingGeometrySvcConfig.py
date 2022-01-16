# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from IOVDbSvc.IOVDbSvcConfig import addFoldersSplitOnline
from SubDetectorEnvelopes.SubDetectorEnvelopesConfigNew import EnvelopeDefSvcCfg 

from GaudiKernel.GaudiHandles import PrivateToolHandleArray

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
    
def _getInDetTrackingGeometryBuilder(name, flags,result, envelopeDefinitionSvc, namePrefix='', setLayerAssociation = True, buildTrtStrawLayers = False):
  # Based on https://gitlab.cern.ch/atlas/athena/blob/master/InnerDetector/InDetDetDescr/InDetTrackingGeometry/python/ConfiguredInDetTrackingGeometryBuilder.py
  # A lot of comments below are to help people understand differences from the above, in case we need to revert some simplifications I made
  # i.e. this is far from complete, but is better than what was there before.
  
  # beampipe
  from BeamPipeGeoModel.BeamPipeGMConfig import BeamPipeGeometryCfg
  result.merge(BeamPipeGeometryCfg(flags))
  InDet__BeamPipeBuilder=CompFactory.InDet.BeamPipeBuilder
  beamPipeBuilder = InDet__BeamPipeBuilder(name=namePrefix+'BeamPipeBuilder')

  result.addPublicTool(beamPipeBuilder)
  layerbuilders = []
  binnings      = []
  colors        = []

  # Pixel
  if flags.Detector.GeometryPixel:
    from AthenaConfiguration.Enums import ProductionStep
    if flags.Common.ProductionStep != ProductionStep.Simulation: # Not needed by FATRAS
        from PixelGeoModel.PixelGeoModelConfig import PixelReadoutGeometryCfg
        result.merge(PixelReadoutGeometryCfg( flags ))
    else:
        from PixelGeoModel.PixelGeoModelConfig import PixelSimulationGeometryCfg
        result.merge(PixelSimulationGeometryCfg( flags ))

    InDet__SiLayerBuilder=CompFactory.InDet.SiLayerBuilder
    PixelLayerBuilder = InDet__SiLayerBuilder(name=namePrefix+'PixelLayerBuilder')
    PixelLayerBuilder.PixelCase 	       = True
    PixelLayerBuilder.Identification       = 'Pixel'
    PixelLayerBuilder.SiDetManagerLocation = 'Pixel'
    # additional layers - handle with care !
    PixelLayerBuilder.BarrelAdditionalLayerRadii      = [ 130.0 ]   # The PST
    PixelLayerBuilder.BarrelAdditionalLayerType       = [ 0 ]     # -- will shift volume boundary to PST
    PixelLayerBuilder.EndcapAdditionalLayerPositionsZ = [ -1900. , 1900. ] # DBM
    PixelLayerBuilder.EndcapAdditionalLayerType       = [  1 , 1 ] # DBM
    # Pixel barrel specifications
    PixelLayerBuilder.BarrelLayerBinsZ     = 1
    PixelLayerBuilder.BarrelLayerBinsPhi   = 1
    PixelLayerBuilder.EndcapLayerBinsR     = 1
    PixelLayerBuilder.EndcapLayerBinsPhi   = 1
    
    # set the layer association
    PixelLayerBuilder.SetLayerAssociation  = setLayerAssociation

    # the binning type of the layers   a
    PixelLayerBinning = 2
    # add it to tool service
    result.addPublicTool(PixelLayerBuilder)
    # put them to the caches
    layerbuilders += [ PixelLayerBuilder ]
    binnings      += [ PixelLayerBinning ]
    colors        += [ 3 ]

  if flags.Detector.GeometrySCT:
    from AthenaConfiguration.Enums import ProductionStep
    if flags.Common.ProductionStep != ProductionStep.Simulation: # Not needed by FATRAS
        from SCT_GeoModel.SCT_GeoModelConfig import SCT_ReadoutGeometryCfg
        result.merge(SCT_ReadoutGeometryCfg( flags ))
    else:
        from SCT_GeoModel.SCT_GeoModelConfig import SCT_SimulationGeometryCfg
        result.merge(SCT_SimulationGeometryCfg( flags ))

    # SCT building
    InDet__SiLayerBuilder=CompFactory.InDet.SiLayerBuilder
    SCT_LayerBuilder = InDet__SiLayerBuilder(name=namePrefix+'SCT_LayerBuilder')
    SCT_LayerBuilder.PixelCase                       = False
    SCT_LayerBuilder.Identification                  = 'SCT'
    SCT_LayerBuilder.SiDetManagerLocation            = 'SCT'
    # additionall layers - handle with care !
    SCT_LayerBuilder.BarrelAdditionalLayerRadii      = []   
    SCT_LayerBuilder.BarrelAdditionalLayerType      = []   

    SCT_LayerBuilder.EndcapAdditionalLayerPositionsZ = [ -2850. , 2850. ] 
    SCT_LayerBuilder.EndcapAdditionalLayerType       = [  0 , 0 ] 
    SCT_LayerBuilder.BarrelLayerBinsZ                = 1
    SCT_LayerBuilder.BarrelLayerBinsPhi              = 1
    # SCT endcap specifications                          
    SCT_LayerBuilder.EndcapLayerBinsR                = 1
    SCT_LayerBuilder.EndcapLayerBinsPhi              = 1
    SCT_LayerBuilder.EndcapComplexRingBinning        = True         
    # set the layer association                   
    SCT_LayerBuilder.SetLayerAssociation             = setLayerAssociation        
    # the binning type of the layer     
    SCT_LayerBinning = 2
    # SCT -> ToolSvc                             
    result.addPublicTool(SCT_LayerBuilder)
                       
    # put them to the caches
    layerbuilders += [ SCT_LayerBuilder ]
    binnings      += [ SCT_LayerBinning ]
    colors        += [ 4 ]

  if flags.Detector.GeometryTRT:     
    from AthenaConfiguration.Enums import ProductionStep
    if flags.Common.ProductionStep != ProductionStep.Simulation: # Not needed by FATRAS
        from TRT_GeoModel.TRT_GeoModelConfig import TRT_ReadoutGeometryCfg
        result.merge(TRT_ReadoutGeometryCfg( flags ))
    else:
        from TRT_GeoModel.TRT_GeoModelConfig import TRT_SimulationGeometryCfg
        result.merge(TRT_SimulationGeometryCfg( flags ))

    InDet__TRT_LayerBuilder=CompFactory.InDet.TRT_LayerBuilder
    TRT_LayerBuilder = InDet__TRT_LayerBuilder(name=namePrefix+'TRT_LayerBuilder')
    # TRT barrel specifications - assume defaults
    # SCT endcap specifications - assume defaults
                       
    # set the binning from bi-aequidistant to arbitrary for complex TRT volumes
    TRT_LayerBinning = 1        
    if buildTrtStrawLayers:
       TRT_LayerBinning = 2
       TRT_LayerBuilder.ModelLayersOnly = False
    # TRT -> ToolSvc                      
    result.addPublicTool(TRT_LayerBuilder)
    
    # put them to the caches
    layerbuilders += [ TRT_LayerBuilder ]
    binnings      += [ TRT_LayerBinning ]
    colors        += [ 5 ]

  # helpers for the InDetTrackingGeometry Builder : layer array creator
  Trk__LayerArrayCreator=CompFactory.Trk.LayerArrayCreator
  InDetLayerArrayCreator = Trk__LayerArrayCreator(name = 'InDetLayerArrayCreator')
  InDetLayerArrayCreator.EmptyLayerMode           = 2 # deletes empty material layers from arrays
  # add to ToolSvc
  result.addPublicTool(InDetLayerArrayCreator)  

  # helpers for the InDetTrackingGeometry Builder : volume array creator
  InDetTrackingVolumeArrayCreator= CompFactory.Trk.TrackingVolumeArrayCreator(name = 'InDetTrackingVolumeArrayCreator')
  # add to ToolSvc
  result.addPublicTool(InDetTrackingVolumeArrayCreator)  

  # helpers for the InDetTrackingGeometry Builder : tracking volume helper for gluing
  Trk__TrackingVolumeHelper=CompFactory.Trk.TrackingVolumeHelper
  InDetTrackingVolumeHelper                             = Trk__TrackingVolumeHelper(name ='InDetTrackingVolumeHelper')
  # the material bins - assume defaults
  # add to ToolSvc
  result.addPublicTool(InDetTrackingVolumeHelper)  
  
  # helpers for the InDetTrackingGeometry Builder : cylinder volume creator
  Trk__CylinderVolumeCreator=CompFactory.Trk.CylinderVolumeCreator
  InDetCylinderVolumeCreator = Trk__CylinderVolumeCreator(name = 'InDetCylinderVolumeCreator')
  # give it the layer array creator
  InDetCylinderVolumeCreator.LayerArrayCreator = InDetLayerArrayCreator
  # specifiy the binning, passive layers, entry layers - assume defaults
  # add to ToolSvc
  result.addPublicTool(InDetCylinderVolumeCreator)  

  # the tracking geometry builder
  InDet__RobustTrackingGeometryBuilder=CompFactory.InDet.RobustTrackingGeometryBuilder
  return InDet__RobustTrackingGeometryBuilder(namePrefix+name,
                                                BeamPipeBuilder   = beamPipeBuilder,
                                                LayerBuilders     = layerbuilders,
                                                LayerBinningType  = binnings,
                                                ColorCodes        = colors,
                                                EnvelopeDefinitionSvc = envelopeDefinitionSvc,
                                                VolumeEnclosureDiscPositions = [ 3000., 3450. ],
                                                TrackingVolumeCreator     = InDetCylinderVolumeCreator,
                                                LayerArrayCreator         = InDetLayerArrayCreator,
                                                ReplaceAllJointBoundaries=True,
                                                VolumeEnclosureCylinderRadii=[],
                                                BuildBoundaryLayers=True,
                                                ExitVolumeName='InDet::Containers::InnerDetector')

                                                # FIXME - not sure what to do about the following:
                                                # BuildBoundaryLayers       = TrkDetFlags.InDetBuildMaterialBoundaries(),
                                                # ReplaceAllJointBoundaries = TrkDetFlags.InDetBuildJointBoundaries(),
                                                # OutputLevel               = TrkDetFlags.InDetBuildingOutputLevel(),
                                                # ExitVolumeName            = TrkDetFlags.InDetContainerName(),
                                                
                                                # Probably they should just be dropped, but I leave this comment for the moment so reviewers can have a think as well.
                                                
                                                # Barrel Entry layers (in old config) etc were removed in 323990adfce581a635ae1809fd2ecc6a093a704c (!)


def _getITkTrackingGeometryBuilder(name, flags,result, envelopeDefinitionSvc, namePrefix='', setLayerAssociation = True, buildTrtStrawLayers = False):
  # Based on https://gitlab.cern.ch/atlas/athena/blob/master/InnerDetector/InDetDetDescr/InDetTrackingGeometry/python/ConfiguredInDetTrackingGeometryBuilder.py
  # A lot of comments below are to help people understand differences from the above, in case we need to revert some simplifications I made
  # i.e. this is far from complete, but is better than what was there before.
  
  # beampipe
  from BeamPipeGeoModel.BeamPipeGMConfig import BeamPipeGeometryCfg
  result.merge(BeamPipeGeometryCfg(flags))
  InDet__BeamPipeBuilder=CompFactory.InDet.BeamPipeBuilder
  beamPipeBuilder = InDet__BeamPipeBuilder(name=namePrefix+'BeamPipeBuilder')
  beamPipeBuilder.BeamPipeMaterialBinsZ              = flags.ITk.trackingGeometry.beampipeMatZbins

  result.addPublicTool(beamPipeBuilder)
  BeamPipeBinning = 2  
  
  Trk__LayerProvider=CompFactory.Trk.LayerProvider
  beamPipeProvider = Trk__LayerProvider(name=namePrefix+'BeamPipeProvider')
  beamPipeProvider.LayerBuilder = beamPipeBuilder
  result.addPublicTool(beamPipeProvider)  
  
  layerProviders = [beamPipeProvider]
  binnings_barrel = [BeamPipeBinning]
  binnings_endcap = [BeamPipeBinning]   
  colors          = [2]
  
  # Pixel
  if flags.Detector.GeometryITkPixel:
    from PixelGeoModelXml.ITkPixelGeoModelConfig import ITkPixelReadoutGeometryCfg
    result.merge(ITkPixelReadoutGeometryCfg(flags))

    InDet__SiLayerBuilder=CompFactory.InDet.SiLayerBuilder
    PixelLayerBuilderInner = InDet__SiLayerBuilder(name=namePrefix+'PixelLayerBuilderInner')
    PixelLayerBuilderInner.PixelCase            = True
    PixelLayerBuilderInner.Identification       = 'ITkPixelInner'
    PixelLayerBuilderInner.SiDetManagerLocation = 'ITkPixel'
    PixelLayerBuilderInner.LayerIndicesBarrel   = [0,1]
    PixelLayerBuilderInner.LayerIndicesEndcap   = [0,1,2]
    PixelLayerBuilderInner.UseRingLayout        = True
    # Pixel barrel specifications
    PixelLayerBuilderInner.BarrelLayerBinsZ     = flags.ITk.trackingGeometry.pixelBarrelMatZbins
    PixelLayerBuilderInner.BarrelLayerBinsPhi   = flags.ITk.trackingGeometry.pixelBarrelMatPhiBins
    PixelLayerBuilderInner.EndcapLayerBinsR     = flags.ITk.trackingGeometry.pixelEndcapMatRbins
    PixelLayerBuilderInner.EndcapLayerBinsPhi   = flags.ITk.trackingGeometry.pixelEndcapMatPhiBins
    
    # set the layer association
    PixelLayerBuilderInner.SetLayerAssociation  = setLayerAssociation

    # the binning type of the layers   a
    PixelLayerBinning = 2
    # add it to tool service
    result.addPublicTool(PixelLayerBuilderInner)
    
    pixelProviderInner = Trk__LayerProvider(name=namePrefix+'PixelProviderInner')
    pixelProviderInner.LayerBuilder = PixelLayerBuilderInner
    result.addPublicTool(pixelProviderInner)
    # put them to the caches
    layerProviders  += [pixelProviderInner]
    binnings_barrel += [ PixelLayerBinning ]
    binnings_endcap += [ PixelLayerBinning ]
    colors          += [ 3 ]
    
    PixelLayerBuilderOuter = InDet__SiLayerBuilder(name=namePrefix+'PixelLayerBuilderOuter')
    PixelLayerBuilderOuter.PixelCase            = True
    PixelLayerBuilderOuter.Identification       = 'ITkPixelOuter'
    PixelLayerBuilderOuter.SiDetManagerLocation = 'ITkPixel'
    PixelLayerBuilderOuter.LayerIndicesBarrel   = [2,3,4]
    PixelLayerBuilderOuter.LayerIndicesEndcap   = [3,4,5,6,7,8]
    PixelLayerBuilderOuter.UseRingLayout=True
    PixelLayerBuilderOuter.BarrelLayerBinsZ     = flags.ITk.trackingGeometry.pixelBarrelMatZbins
    PixelLayerBuilderOuter.BarrelLayerBinsPhi   = flags.ITk.trackingGeometry.pixelBarrelMatPhiBins
    PixelLayerBuilderOuter.EndcapLayerBinsR     = flags.ITk.trackingGeometry.pixelEndcapMatRbins
    PixelLayerBuilderOuter.EndcapLayerBinsPhi   = flags.ITk.trackingGeometry.pixelEndcapMatPhiBins
    
    # set the layer association
    PixelLayerBuilderOuter.SetLayerAssociation  = setLayerAssociation

    # the binning type of the layers   a
    PixelLayerBinning = 2
    # add it to tool service
    result.addPublicTool(PixelLayerBuilderOuter)
    
    pixelProviderOuter = Trk__LayerProvider(name=namePrefix+'PixelProviderOuter')
    pixelProviderOuter.LayerBuilder = PixelLayerBuilderOuter
    result.addPublicTool(pixelProviderOuter)
    # put them to the caches
    layerProviders  += [pixelProviderOuter]
    binnings_barrel += [ PixelLayerBinning ]
    binnings_endcap += [ PixelLayerBinning ]
    colors          += [ 3 ]

  if flags.Detector.GeometryITkStrip:
    from StripGeoModelXml.ITkStripGeoModelConfig import ITkStripReadoutGeometryCfg
    result.merge(ITkStripReadoutGeometryCfg(flags))

    # SCT building
    InDet__SiLayerBuilder=CompFactory.InDet.SiLayerBuilder
    SCT_LayerBuilder = InDet__SiLayerBuilder(name=namePrefix+'SCT_LayerBuilder')
    SCT_LayerBuilder.PixelCase                       = False
    SCT_LayerBuilder.Identification                  = 'ITkStrip'
    SCT_LayerBuilder.SiDetManagerLocation            = 'ITkStrip'
    SCT_LayerBuilder.AddMoreSurfaces                 = True
    SCT_LayerBuilder.BarrelLayerBinsZ                = flags.ITk.trackingGeometry.stripBarrelMatZbins
    SCT_LayerBuilder.BarrelLayerBinsPhi              = flags.ITk.trackingGeometry.stripBarrelMatPhiBins
    SCT_LayerBuilder.EndcapLayerBinsR                = flags.ITk.trackingGeometry.stripEndcapMatRbins
    SCT_LayerBuilder.EndcapLayerBinsPhi              = flags.ITk.trackingGeometry.stripEndcapMatPhiBins
    # set the layer association                   
    SCT_LayerBuilder.SetLayerAssociation             = setLayerAssociation        
    # the binning type of the layer     
    SCT_LayerBinning = 2
    # SCT -> ToolSvc                             
    result.addPublicTool(SCT_LayerBuilder)
    
    stripProvider = Trk__LayerProvider(name=namePrefix+'StripProvider')
    stripProvider.LayerBuilder = SCT_LayerBuilder
    result.addPublicTool(stripProvider)
    
    # put them to the caches
    layerProviders  += [stripProvider]
    binnings_barrel += [ SCT_LayerBinning ]
    binnings_endcap += [ SCT_LayerBinning ]
    colors          += [ 4 ]
  
  # helpers for the InDetTrackingGeometry Builder : layer array creator
  Trk__LayerArrayCreator=CompFactory.Trk.LayerArrayCreator
  InDetLayerArrayCreator = Trk__LayerArrayCreator(name = 'InDetLayerArrayCreator')
  InDetLayerArrayCreator.EmptyLayerMode           = 2 # deletes empty material layers from arrays
  # add to ToolSvc
  result.addPublicTool(InDetLayerArrayCreator)  

  # helpers for the InDetTrackingGeometry Builder : volume array creator
  Trk__TrackingVolumeArrayCreator= CompFactory.Trk.TrackingVolumeArrayCreator
  InDetTrackingVolumeArrayCreator = Trk__TrackingVolumeArrayCreator(name = 'InDetTrackingVolumeArrayCreator')
  # add to ToolSvc
  result.addPublicTool(InDetTrackingVolumeArrayCreator)  

  # helpers for the InDetTrackingGeometry Builder : tracking volume helper for gluing
  Trk__TrackingVolumeHelper=CompFactory.Trk.TrackingVolumeHelper
  InDetTrackingVolumeHelper                             = Trk__TrackingVolumeHelper(name ='InDetTrackingVolumeHelper')
  InDetTrackingVolumeHelper.BarrelLayerBinsZ   = flags.ITk.trackingGeometry.passiveBarrelMatZbins
  InDetTrackingVolumeHelper.BarrelLayerBinsPhi = flags.ITk.trackingGeometry.passiveBarrelMatPhiBins
  InDetTrackingVolumeHelper.EndcapLayerBinsR   = flags.ITk.trackingGeometry.passiveEndcapMatRbins
  InDetTrackingVolumeHelper.EndcapLayerBinsPhi = flags.ITk.trackingGeometry.passiveEndcapMatPhiBins
  
  # the material bins - assume defaults
  # add to ToolSvc
  result.addPublicTool(InDetTrackingVolumeHelper)  
  
  # helpers for the InDetTrackingGeometry Builder : cylinder volume creator
  Trk__CylinderVolumeCreator=CompFactory.Trk.CylinderVolumeCreator
  InDetCylinderVolumeCreator = Trk__CylinderVolumeCreator(name = 'InDetCylinderVolumeCreator')
  # give it the layer array creator
  InDetCylinderVolumeCreator.LayerArrayCreator = InDetLayerArrayCreator
  InDetCylinderVolumeCreator.TrackingVolumeArrayCreator = InDetTrackingVolumeArrayCreator
  InDetCylinderVolumeCreator.TrackingVolumeHelper       = InDetTrackingVolumeHelper
  InDetCylinderVolumeCreator.PassiveLayerThickness      = 1. ## mm
  InDetCylinderVolumeCreator.PassiveLayerBinsRZ   = flags.ITk.trackingGeometry.passiveBarrelMatZbins
  InDetCylinderVolumeCreator.PassiveLayerBinsPhi  = flags.ITk.trackingGeometry.passiveBarrelMatPhiBins
        
  # specifiy the binning, passive layers, entry layers - assume defaults
  # add to ToolSvc
  result.addPublicTool(InDetCylinderVolumeCreator)  

  # the tracking geometry builder
  InDet__StagedTrackingGeometryBuilder=CompFactory.InDet.StagedTrackingGeometryBuilder
  return InDet__StagedTrackingGeometryBuilder(namePrefix+name,
                                              LayerBuilders     = layerProviders,
                                              LayerBinningTypeCenter    = binnings_barrel,
                                              LayerBinningTypeEndcap    = binnings_endcap,
                                              ColorCodes                = colors,
                                              EnvelopeDefinitionSvc     = envelopeDefinitionSvc,
                                              TrackingVolumeCreator     = InDetCylinderVolumeCreator,
                                              LayerArrayCreator         = InDetLayerArrayCreator,
                                              CheckForRingLayout        = True,
                                              MinimalRadialGapForVolumeSplit = flags.ITk.trackingGeometry.minimalRadialGapForVolumeSplit,
                                              ReplaceAllJointBoundaries = True,
                                              BuildBoundaryLayers=True,
                                              ExitVolumeName='InDet::Containers::InnerDetector')
  
# Replaces https://gitlab.cern.ch/atlas/athena/blob/master/Calorimeter/CaloTrackingGeometry/python/ConfiguredCaloTrackingGeometryBuilder.py
def _getCaloTrackingGeometryBuilder(name, flags,result, envelopeDefinitionSvc, trackingVolumeHelper, namePrefix=''):
  # The following replaces LArCalorimeter/LArTrackingGeometry/python/ConfiguredLArVolumeBuilder.py
  from LArGeoAlgsNV.LArGMConfig import LArGMCfg
  result.merge(LArGMCfg(flags))
  LAr__LArVolumeBuilder=CompFactory.LAr.LArVolumeBuilder
  lArVolumeBuilder = LAr__LArVolumeBuilder(name="LArVolumeBuilderCond", TrackingVolumeHelper = trackingVolumeHelper,)
  result.addPublicTool(lArVolumeBuilder)
  
  # The following replaces TileCalorimeter/TileTrackingGeometry/python/ConfiguredTileVolumeBuilder.py
  from TileGeoModel.TileGMConfig import TileGMCfg
  result.merge(TileGMCfg(flags))
  Tile__TileVolumeBuilder=CompFactory.Tile.TileVolumeBuilder
  tileVolumeBuilder = Tile__TileVolumeBuilder(name="TileVolumeBuilderCond", TrackingVolumeHelper = trackingVolumeHelper,  )
  result.addPublicTool(tileVolumeBuilder)
  
  Calo__CaloTrackingGeometryBuilder=CompFactory.Calo.CaloTrackingGeometryBuilder
  return Calo__CaloTrackingGeometryBuilder(namePrefix+name, LArVolumeBuilder = lArVolumeBuilder,
                                                   TileVolumeBuilder = tileVolumeBuilder,
                                                   TrackingVolumeHelper = trackingVolumeHelper,
                                                   EnvelopeDefinitionSvc = envelopeDefinitionSvc,
                                                   EntryVolumeName = "InDet::Containers::EntryVolume",
                                                   ExitVolumeName = "Calo::Container",
                                                   GapLayerEnvelope=5.0
                                                   )

# Originally this function would use was TrkDetFlags.MaterialSource() and TrkDetFlags.MaterialValidation(). For new configuration, (temporarily?) pass as parameters.
# https://gitlab.cern.ch/atlas/athena/blob/master/Tracking/TrkDetDescr/TrkDetDescrSvc/python/AtlasTrackingGeometrySvc.py#L112
def TrackingGeometrySvcCfg( flags , name = 'AtlasTrackingGeometrySvc', doMaterialValidation=False ) :
    """
    Sets up the Tracking Geometry Service
    """
    result = ComponentAccumulator()
    atlas_tracking_geometry_name = 'AtlasTrackingGeometry'
    atlas_geometry_builder = CompFactory.Trk.GeometryBuilder(name = 'AtlasGeometryBuilder', WorldDimension=[], WorldMaterialProperties=[], 
                                TrackingVolumeArrayCreator=CompFactory.Trk.TrackingVolumeArrayCreator(name="TrackingVolumeArrayCreator"), 
                                TrackingVolumeHelper = CompFactory.Trk.TrackingVolumeHelper(name="TrackingVolumeHelper") )
    
    envelopSvcCfg= EnvelopeDefSvcCfg(flags)
    atlas_env_def_service=envelopSvcCfg.getPrimary()
    result.merge(envelopSvcCfg)

    # Depending on the job configuration, setup the various detector builders, and add to atlas_geometry_builder
    if flags.Detector.GeometryID:
      # TODO Not sure how to handle TrkDetFlags, specifically ISF_FatrasCustomGeometry, XMLFastCustomGeometry
      # So, here we only setup the default InDet geometry builder!
      inDetTrackingGeometryBuilder = _getInDetTrackingGeometryBuilder(name ='InDetTrackingGeometryBuilder', flags=flags, result=result, envelopeDefinitionSvc=atlas_env_def_service,
                                                                      buildTrtStrawLayers=flags.Beam.Type=='cosmics')
      
      atlas_geometry_builder.InDetTrackingGeometryBuilder = inDetTrackingGeometryBuilder
      
    elif flags.Detector.GeometryITk:
      inDetTrackingGeometryBuilder = _getITkTrackingGeometryBuilder(name ='InDetTrackingGeometryBuilder', flags=flags, result=result,
                                                                    envelopeDefinitionSvc=atlas_env_def_service)
      atlas_geometry_builder.InDetTrackingGeometryBuilder = inDetTrackingGeometryBuilder
      
    if flags.Detector.GeometryCalo:
      Trk__CylinderVolumeCreator=CompFactory.Trk.CylinderVolumeCreator
      caloVolumeCreator = Trk__CylinderVolumeCreator("CaloVolumeCreator")
      result.addPublicTool(caloVolumeCreator)

      Trk__TrackingVolumeHelper=CompFactory.Trk.TrackingVolumeHelper
      trackingVolumeHelper = Trk__TrackingVolumeHelper(name='TrackingVolumeHelper')
      result.addPublicTool(trackingVolumeHelper)

      caloTrackingGeometryBuilder = _getCaloTrackingGeometryBuilder(name ='CaloTrackingGeometryBuilder', flags=flags, 
                                     result=result, envelopeDefinitionSvc=atlas_env_def_service, trackingVolumeHelper=trackingVolumeHelper)
      caloTrackingGeometryBuilder.TrackingVolumeCreator=caloVolumeCreator
      atlas_geometry_builder.CaloTrackingGeometryBuilder = caloTrackingGeometryBuilder

    if flags.Detector.GeometryMuon:
      # Copied from from MuonTrackingGeometry.ConfiguredMuonTrackingGeometry import MuonTrackingGeometryBuilder
      # Add the muon geometry model to the CA
      from MuonConfig.MuonGeometryConfig import MuonGeoModelCfg
      result.merge(MuonGeoModelCfg(flags))
      Muon__MuonStationTypeBuilder=CompFactory.Muon.MuonStationTypeBuilder
      muonStationTypeBuilder= Muon__MuonStationTypeBuilder(name = 'MuonStationTypeBuilder')
    
      Muon__MuonStationBuilder=CompFactory.Muon.MuonStationBuilder
      muonStationBuilder= Muon__MuonStationBuilder(name = 'MuonStationBuilder')
      muonStationBuilder.StationTypeBuilder = muonStationTypeBuilder

      muonInertMaterialBuilder= CompFactory.Muon.MuonInertMaterialBuilder(name = 'MuonInertMaterialBuilder')

      Muon__MuonTrackingGeometryBuilder=CompFactory.Muon.MuonTrackingGeometryBuilder
      muonTrackingGeometryBuilder= Muon__MuonTrackingGeometryBuilder(name = 'MuonTrackingGeometryBuilder', 
                                                                     EnvelopeDefinitionSvc=atlas_env_def_service,
                                                                     MuonStationBuilder = muonStationBuilder,
                                                                     InertMaterialBuilder=muonInertMaterialBuilder
                                                                     )
      # import pdb ; pdb.set_trace()
      muonTrackingGeometryBuilder.EntryVolumeName = 'MuonSpectrometerEntrance'
      muonTrackingGeometryBuilder.ExitVolumeName  = 'Muon::Containers::MuonSystem'
      # result.addPublicTool(muonTrackingGeometryBuilder)
    
      atlas_geometry_builder.MuonTrackingGeometryBuilder = muonTrackingGeometryBuilder
    
    # Now set up processors
    atlas_geometry_processors=[]
    
    if flags.TrackingGeometry.MaterialSource == 'COOL':
      
      CoolDataBaseFolder = '/GLOBAL/TrackingGeo/LayerMaterialV2'
      if flags.Detector.GeometryITk:
        CoolDataBaseFolder = '/GLOBAL/TrackingGeo/LayerMaterialITK'
      
      # the material provider
      Trk__LayerMaterialProvider=CompFactory.Trk.LayerMaterialProvider
      atlasMaterialProvider = Trk__LayerMaterialProvider('AtlasMaterialProvider', LayerMaterialMapName=CoolDataBaseFolder, LayerMaterialMapKey='')
      atlas_geometry_processors += [ atlasMaterialProvider ]
        
      # Setup DBs
      result.merge(_setupCondDB(flags, CoolDataBaseFolder))
       
    elif  flags.TrackingGeometry.MaterialSource == 'Input':
      Trk__InputLayerMaterialProvider=CompFactory.Trk.InputLayerMaterialProvider
      atlasMaterialProvider = Trk__InputLayerMaterialProvider('AtlasMaterialProvider')
      atlas_geometry_processors += [ atlasMaterialProvider ]
      
    if doMaterialValidation:
      Trk__LayerMaterialInspector=CompFactory.Trk.LayerMaterialInspector
      atlasLayerMaterialInspector = Trk__LayerMaterialInspector('AtlasLayerMaterialInspector')
      atlas_geometry_processors += [ atlasLayerMaterialInspector ]

    svc = CompFactory.Trk.TrackingGeometrySvc( name, GeometryBuilder = atlas_geometry_builder,
                                    TrackingGeometryName = atlas_tracking_geometry_name,
                                    GeometryProcessors = PrivateToolHandleArray(atlas_geometry_processors), 
                                    BuildGeometryFromTagInfo = True)
    result.addService(svc, primary = True)

    return result
        
if __name__ == '__main__':
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior=1
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    from AthenaConfiguration.TestDefaults import defaultTestFiles

    ConfigFlags.Input.Files = defaultTestFiles.RAW
    ConfigFlags.lock()

    acc = TrackingGeometrySvcCfg(ConfigFlags )

    f=open('TrackingGeometrySvcCfg.pkl','wb')
    acc.store(f)
    f.close()
