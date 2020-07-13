# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
Trk__TrackingGeometryCondAlg=CompFactory.Trk__TrackingGeometryCondAlg
from IOVDbSvc.IOVDbSvcConfig import addFoldersSplitOnline
from SubDetectorEnvelopes.SubDetectorEnvelopesConfig import getEnvelopeDefSvc

# This file is a placeholder - the entire way we build geometry needs to be rewritten so this is to unblock new configuration developments for the moment.
# It is based on: https://gitlab.cern.ch/atlas/athena/blob/master/Tracking/TrkDetDescr/TrkDetDescrSvc/python/AtlasTrackingGeometrySvc.py#L112

def _setupCondDB(flags, CoolDataBaseFolder, quiet=True):
    result=ComponentAccumulator()
    
    # the tag names
    materialTagBase = 'AtlasLayerMat_v'
    version = 21
    sub_version = ''
    
    AtlasMaterialTag = materialTagBase+str(version)+sub_version+'_'
    cfolder = CoolDataBaseFolder +'<tag>TagInfoMajor/'+AtlasMaterialTag+'/GeoAtlas</tag>'

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
  InDet__BeamPipeBuilder=CompFactory.InDet__BeamPipeBuilderCond
  beamPipeBuilder = InDet__BeamPipeBuilder(name=namePrefix+'BeamPipeBuilder')

  result.addPublicTool(beamPipeBuilder)
  layerbuilders = []
  binnings      = []
  colors        = []

  # Pixel
  if flags.Detector.GeometryPixel:
    InDet__SiLayerBuilder=CompFactory.InDet__SiLayerBuilderCond
    PixelLayerBuilder = InDet__SiLayerBuilder(name=namePrefix+'PixelLayerBuilder')
    PixelLayerBuilder.PixelCase            = True
    PixelLayerBuilder.Identification       = 'Pixel'
    PixelLayerBuilder.SiDetManagerLocation = 'Pixel'
    # additional layers - handle with care !
    PixelLayerBuilder.BarrelAdditionalLayerRadii      = [ 130 ]   # The PST
    PixelLayerBuilder.BarrelAdditionalLayerType       = [ 0 ]     # -- will shift volume boundary to PST
    PixelLayerBuilder.EndcapAdditionalLayerPositionsZ = [ -1900. , 1900. ] # DBM
    PixelLayerBuilder.EndcapAdditionalLayerType       = [  1 , 1 ] # DBM
    # Pixel barrel specifications
    # Since we don't have TrkDetFlags, using defaults here.
    
    # set the layer association
    PixelLayerBuilder.SetLayerAssociation  = setLayerAssociation

    # the binning type of the layers   
    PixelLayerBinning = 2
    # add it to tool service
    result.addPublicTool(PixelLayerBuilder)
    # put them to the caches
    layerbuilders += [ PixelLayerBuilder ]
    binnings      += [ PixelLayerBinning ]
    colors        += [ 3 ]

  if flags.Detector.GeometrySCT:
    # SCT building
    InDet__SiLayerBuilder=CompFactory.InDet__SiLayerBuilderCond
    SCT_LayerBuilder = InDet__SiLayerBuilder(name=namePrefix+'SCT_LayerBuilder')
    SCT_LayerBuilder.PixelCase                       = False
    SCT_LayerBuilder.Identification                  = 'SCT'
    SCT_LayerBuilder.SiDetManagerLocation            = 'SCT'
    # additionall layers - handle with care !
    SCT_LayerBuilder.EndcapAdditionalLayerPositionsZ = [ -2850 , 2850 ] 
    SCT_LayerBuilder.EndcapAdditionalLayerType       = [  0 , 0 ] 
    # SCT barrel specifications - use defaults
    # SCT endcap specifications - use defaults           
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
    InDet__TRT_LayerBuilder=CompFactory.InDet__TRT_LayerBuilderCond
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
  Trk__LayerArrayCreator=CompFactory.Trk__LayerArrayCreator
  InDetLayerArrayCreator = Trk__LayerArrayCreator(name = 'InDetLayerArrayCreator')
  InDetLayerArrayCreator.EmptyLayerMode           = 2 # deletes empty material layers from arrays
  # add to ToolSvc
  result.addPublicTool(InDetLayerArrayCreator)  

  # helpers for the InDetTrackingGeometry Builder : volume array creator
  Trk__TrackingVolumeArrayCreator=CompFactory.Trk__TrackingVolumeArrayCreator
  InDetTrackingVolumeArrayCreator                       = Trk__TrackingVolumeArrayCreator(name = 'InDetTrackingVolumeArrayCreator')
  # add to ToolSvc
  result.addPublicTool(InDetTrackingVolumeArrayCreator)  

  # helpers for the InDetTrackingGeometry Builder : tracking voluem helper for glueing
  Trk__TrackingVolumeHelper=CompFactory.Trk__TrackingVolumeHelper
  InDetTrackingVolumeHelper                             = Trk__TrackingVolumeHelper(name ='InDetTrackingVolumeHelper')
  # the material bins - assume defaults
  # add to ToolSvc
  result.addPublicTool(InDetTrackingVolumeHelper)  
  
  # helpers for the InDetTrackingGeometry Builder : cylinder volume creator
  Trk__CylinderVolumeCreator=CompFactory.Trk__CylinderVolumeCreator
  InDetCylinderVolumeCreator = Trk__CylinderVolumeCreator(name = 'InDetCylinderVolumeCreator')
  # give it the layer array creator - assume defaults
  # specifiy the binning, passive layers, entry layers - assume defaults
  # add to ToolSvc
  result.addPublicTool(InDetCylinderVolumeCreator)  

  # the tracking geometry builder
  InDet__RobustTrackingGeometryBuilder=CompFactory.InDet__RobustTrackingGeometryBuilderCond
  return InDet__RobustTrackingGeometryBuilder(namePrefix+name,
                                                BeamPipeBuilder   = beamPipeBuilder,
                                                LayerBuilders     = layerbuilders,
                                                LayerBinningType  = binnings,
                                                ColorCodes        = colors,
                                                EnvelopeDefinitionSvc = envelopeDefinitionSvc,
                                                VolumeEnclosureDiscPositions = [ 3000., 3450. ],
                                                TrackingVolumeCreator     = InDetCylinderVolumeCreator,
                                                LayerArrayCreator         = InDetLayerArrayCreator)

                                                # FIXME - not sure what to do about the following:
                                                # BuildBoundaryLayers       = TrkDetFlags.InDetBuildMaterialBoundaries(),
                                                # ReplaceAllJointBoundaries = TrkDetFlags.InDetBuildJointBoundaries(),
                                                # OutputLevel               = TrkDetFlags.InDetBuildingOutputLevel(),
                                                # ExitVolumeName            = TrkDetFlags.InDetContainerName(),
                                                
                                                # Probably they should just be dropped, but I leave this comment for the moment so reviewers can have a think as well.
                                                
                                                # Barrel Entry layers (in old config) etc were removed in 323990adfce581a635ae1809fd2ecc6a093a704c (!)
  
# Replaces https://gitlab.cern.ch/atlas/athena/blob/master/Calorimeter/CaloTrackingGeometry/python/ConfiguredCaloTrackingGeometryBuilder.py
def _getCaloTrackingGeometryBuilder(name, flags,result, envelopeDefinitionSvc, trackingVolumeHelper, namePrefix=''):
  # The following replaces LArCalorimeter/LArTrackingGeometry/python/ConfiguredLArVolumeBuilder.py
  LAr__LArVolumeBuilder=CompFactory.LAr__LArVolumeBuilder
  lArVolumeBuilder = LAr__LArVolumeBuilder(TrackingVolumeHelper = trackingVolumeHelper,)
  result.addPublicTool(lArVolumeBuilder)
  
  # The following replaces TileCalorimeter/TileTrackingGeometry/python/ConfiguredTileVolumeBuilder.py
  Tile__TileVolumeBuilder=CompFactory.Tile__TileVolumeBuilder
  tileVolumeBuilder = Tile__TileVolumeBuilder( TrackingVolumeHelper = trackingVolumeHelper,  )
  result.addPublicTool(tileVolumeBuilder)
  
  Calo__CaloTrackingGeometryBuilder=CompFactory.Calo__CaloTrackingGeometryBuilderCond
  return Calo__CaloTrackingGeometryBuilder(namePrefix+name, LArVolumeBuilder = lArVolumeBuilder,
                                                   TileVolumeBuilder = tileVolumeBuilder,
                                                   TrackingVolumeHelper = trackingVolumeHelper,
                                                   EnvelopeDefinitionSvc = envelopeDefinitionSvc,
                                                   )

# Originally this function would use was TrkDetFlags.MaterialSource() and TrkDetFlags.MaterialValidation(). For new configuration, (temporarily?) pass as parameters.
# https://gitlab.cern.ch/atlas/athena/blob/master/Tracking/TrkDetDescr/TrkDetDescrSvc/python/AtlasTrackingGeometrySvc.py#L112
def TrackingGeometryCondAlgCfg( flags , name = 'AtlasTrackingGeometryCondAlg', doMaterialValidation=False ) :
    """
    Sets up the Tracking Geometry Conditions Algorithm
    """
    result = ComponentAccumulator()
    atlas_tracking_geometry_name = 'AtlasTrackingGeometry'
    Trk__GeometryBuilder=CompFactory.Trk__GeometryBuilderCond
    atlas_geometry_builder = Trk__GeometryBuilder(name = 'AtlasGeometryBuilder')

    atlas_env_def_service = getEnvelopeDefSvc()
    result.addService(atlas_env_def_service)  

    # Depending on the job configuration, setup the various detector builders, and add to atlas_geometry_builder
    if flags.Detector.GeometryID:
      # TODO Not sure how to handle TrkDetFlags, specifically ISF_FatrasCustomGeometry, XMLFastCustomGeometry, SLHC_Geometry
      # So, here we only setup the default InDet geometry builder!
      inDetTrackingGeometryBuilder = _getInDetTrackingGeometryBuilder(name ='InDetTrackingGeometryBuilder', flags=flags, result=result, envelopeDefinitionSvc=atlas_env_def_service)
      result.addPublicTool(inDetTrackingGeometryBuilder)
      
      atlas_geometry_builder.InDetTrackingGeometryBuilder = inDetTrackingGeometryBuilder
      
    if flags.Detector.GeometryCalo:
      Trk__CylinderVolumeCreator=CompFactory.Trk__CylinderVolumeCreator
      caloVolumeCreator = Trk__CylinderVolumeCreator("CaloVolumeCreator")
      result.addPublicTool(caloVolumeCreator)

      Trk__TrackingVolumeHelper=CompFactory.Trk__TrackingVolumeHelper
      trackingVolumeHelper = Trk__TrackingVolumeHelper(name='TrackingVolumeHelper')
      result.addPublicTool(trackingVolumeHelper)

      caloTrackingGeometryBuilder = _getCaloTrackingGeometryBuilder(name ='CaloTrackingGeometryBuilder', flags=flags, result=result, envelopeDefinitionSvc=atlas_env_def_service, trackingVolumeHelper=trackingVolumeHelper)
      result.addPublicTool(caloTrackingGeometryBuilder)
      atlas_geometry_builder.CaloTrackingGeometryBuilder = caloTrackingGeometryBuilder

    if flags.Detector.GeometryMuon:
      # Copied from from MuonTrackingGeometry.ConfiguredMuonTrackingGeometry import MuonTrackingGeometryBuilder
      Muon__MuonStationTypeBuilder=CompFactory.Muon__MuonStationTypeBuilder
      muonStationTypeBuilder= Muon__MuonStationTypeBuilder(name = 'MuonStationTypeBuilder')
      result.addPublicTool(muonStationTypeBuilder)
    
      Muon__MuonStationBuilder=CompFactory.Muon__MuonStationBuilder
      muonStationBuilder= Muon__MuonStationBuilder(name = 'MuonStationBuilder')
      muonStationBuilder.StationTypeBuilder = muonStationTypeBuilder
      result.addPublicTool(muonStationBuilder)

      Muon__MuonInertMaterialBuilder=CompFactory.Muon__MuonInertMaterialBuilder
      muonInertMaterialBuilder= Muon__MuonInertMaterialBuilder(name = 'MuonInertMaterialBuilder')
      result.addPublicTool(muonInertMaterialBuilder)

      Muon__MuonTrackingGeometryBuilder=CompFactory.Muon__MuonTrackingGeometryBuilderCond
      muonTrackingGeometryBuilder= Muon__MuonTrackingGeometryBuilder(name = 'MuonTrackingGeometryBuilder', EnvelopeDefinitionSvc=atlas_env_def_service)
      result.addPublicTool(muonTrackingGeometryBuilder)
    
      atlas_geometry_builder.MuonTrackingGeometryBuilder = muonTrackingGeometryBuilder
    
    # Now set up processors
    atlas_geometry_processors=[]
    
    if flags.TrackingGeometry.MaterialSource == 'COOL':
       CoolDataBaseFolder = '/GLOBAL/TrackingGeo/LayerMaterialV2' # Was from TrkDetFlags.MaterialStoreGateKey()
       # the material provider
       Trk__LayerMaterialProvider=CompFactory.Trk__LayerMaterialProvider
       atlasMaterialProvider = Trk__LayerMaterialProvider('AtlasMaterialProvider', LayerMaterialMapName=CoolDataBaseFolder)
       atlas_geometry_processors += [ atlasMaterialProvider ]

       # Setup DBs
       result.merge(_setupCondDB(flags, CoolDataBaseFolder))
    elif  flags.TrackingGeometry.MaterialSource == 'Input':
      Trk__InputLayerMaterialProvider=CompFactory.Trk__InputLayerMaterialProvider
      atlasMaterialProvider = Trk__InputLayerMaterialProvider('AtlasMaterialProvider')
      atlas_geometry_processors += [ atlasMaterialProvider ]
      
    if doMaterialValidation:
      Trk__LayerMaterialInspector=CompFactory.Trk__LayerMaterialInspector
      atlasLayerMaterialInspector = Trk__LayerMaterialInspector('AtlasLayerMaterialInspector')
      atlas_geometry_processors += [ atlasLayerMaterialInspector ]

    condAlg = Trk__TrackingGeometryCondAlg( name, GeometryBuilder = atlas_geometry_builder,
                                    TrackingGeometryWriteKey = atlas_tracking_geometry_name,
                                    GeometryProcessors = atlas_geometry_processors, 
                                    BuildGeometryFromTagInfo = True)
    result.addCondAlgo(condAlg, primary = True)

    return result
        
if __name__ == '__main__':
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior=1
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    from AthenaConfiguration.TestDefaults import defaultTestFiles

    ConfigFlags.Input.Files = defaultTestFiles.RAW
    ConfigFlags.Detector.GeometryPixel = True     
    ConfigFlags.Detector.GeometrySCT   = True 
    ConfigFlags.Detector.GeometryTRT   = True 
    ConfigFlags.Detector.GeometryLAr   = True     
    ConfigFlags.Detector.GeometryTile  = True     
    ConfigFlags.Detector.GeometryMDT   = True 
    ConfigFlags.Detector.GeometryTGC   = True
    ConfigFlags.Detector.GeometryCSC   = True     
    ConfigFlags.Detector.GeometryRPC   = True 
    ConfigFlags.lock()

    acc = TrackingGeometryCondAlgCfg(ConfigFlags )

    f=open('TrackingGeometryCondAlgCfg.pkl','wb')
    acc.store(f)
    f.close()
