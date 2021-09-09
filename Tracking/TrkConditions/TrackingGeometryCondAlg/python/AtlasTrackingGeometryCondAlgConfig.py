# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
Trk__TrackingGeometryCondAlg=CompFactory.Trk.TrackingGeometryCondAlg
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
    cfolder = CoolDataBaseFolder +'<tag>TagInfoMajor/'+AtlasMaterialTag+'/GeoAtlas</tag>'

    # if not quiet:
    #   print('[ TrackingGeometrySvc ]     base material tag : ' + AtlasMaterialTag)
    #   print('[ TrackingGeometrySvc ]     translated to COOL: ' + cfolder)

    # load the right folders
    result.merge( addFoldersSplitOnline(flags,'GLOBAL',[cfolder],[cfolder],splitMC=True,
                                        className = 'Trk::LayerMaterialMap') )
    return result
    
def _getInDetTrackingGeometryBuilder(name, flags,result, envelopeDefinitionSvc, namePrefix='', nameSuffix='',setLayerAssociation = True, buildTrtStrawLayers = False):
  # Based on https://gitlab.cern.ch/atlas/athena/blob/master/InnerDetector/InDetDetDescr/InDetTrackingGeometry/python/ConfiguredInDetTrackingGeometryBuilder.py
  # A lot of comments below are to help people understand differences from the above, in case we need to revert some simplifications I made
  # i.e. this is far from complete, but is better than what was there before.

  # beampipe
  from BeamPipeGeoModel.BeamPipeGMConfig import BeamPipeGeometryCfg
  result.merge(BeamPipeGeometryCfg(flags))
  InDet__BeamPipeBuilder=CompFactory.InDet.BeamPipeBuilderCond
  beamPipeBuilder = InDet__BeamPipeBuilder(name=namePrefix+'BeamPipeBuilder'+nameSuffix)

  result.addPublicTool(beamPipeBuilder)
  layerbuilders = []
  binnings      = []
  colors        = []

  # Pixel
  if flags.Detector.GeometryPixel:
    # for Pixel DetectorElement conditions data :
    from PixelGeoModel.PixelGeoModelConfig import PixelGeometryCfg
    result.merge(PixelGeometryCfg( flags ))

    InDet__SiLayerBuilder=CompFactory.InDet.SiLayerBuilderCond
    PixelLayerBuilder = InDet__SiLayerBuilder(name=namePrefix+'PixelLayerBuilder'+nameSuffix)
    PixelLayerBuilder.PixelCase            = True
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

    # the binning type of the layers   
    PixelLayerBinning = 2
    # add it to tool service
    result.addPublicTool(PixelLayerBuilder)
    # put them to the caches
    layerbuilders += [ PixelLayerBuilder ]
    binnings      += [ PixelLayerBinning ]
    colors        += [ 3 ]

    # add artifical dependencies to Pixel DetectorElement conditions algs to ensure that the IOV
    # is identical to the IOV of the tracking geoemtry cond alg
    from PixelConditionsAlgorithms.PixelConditionsConfig import PixelDetectorElementCondAlgCfg
    result.merge(PixelDetectorElementCondAlgCfg(flags,
                                                MuonManagerKey    = ["MuonDetectorManager"]     if flags.Muon.enableAlignment and flags.Detector.GeometryMuon else [],
                                                TRT_DetEltContKey = ["TRT_DetElementContainer"] if flags.Detector.GeometryTRT  else [],
                                                SCTAlignmentStore = ["SCTAlignmentStore"]       if flags.Detector.GeometrySCT  else []))

  if flags.Detector.GeometrySCT:
    # for SCT DetectorElement conditions data :
    from SCT_GeoModel.SCT_GeoModelConfig import SCT_GeometryCfg
    result.merge(SCT_GeometryCfg( flags ))

    # SCT building
    InDet__SiLayerBuilder=CompFactory.InDet.SiLayerBuilderCond
    SCT_LayerBuilder = InDet__SiLayerBuilder(name=namePrefix+'SCT_LayerBuilder'+nameSuffix)
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

    from SCT_GeoModel.SCT_GeoModelConfig import SCT_DetectorElementCondAlgCfg
    result.merge(SCT_DetectorElementCondAlgCfg(flags,
                                                MuonManagerKey      = ["MuonDetectorManager"]     if flags.Muon.enableAlignment and  flags.Detector.GeometryMuon else [],
                                                TRT_DetEltContKey   = ["TRT_DetElementContainer"] if flags.Detector.GeometryTRT   else [],
                                                PixelAlignmentStore = ["PixelAlignmentStore"]   if flags.Detector.GeometryPixel else []))

  if flags.Detector.GeometryTRT:
    # for TRT DetectorElement conditions data :
    from TRT_GeoModel.TRT_GeoModelConfig import TRT_GeometryCfg
    result.merge(TRT_GeometryCfg( flags ))

    InDet__TRT_LayerBuilder=CompFactory.InDet.TRT_LayerBuilderCond
    TRT_LayerBuilder = InDet__TRT_LayerBuilder(name=namePrefix+'TRT_LayerBuilder'+nameSuffix)
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
  InDetLayerArrayCreator = Trk__LayerArrayCreator(name = namePrefix+'InDetLayerArrayCreator'+nameSuffix)
  InDetLayerArrayCreator.EmptyLayerMode           = 2 # deletes empty material layers from arrays
  # add to ToolSvc
  result.addPublicTool(InDetLayerArrayCreator)  

  # helpers for the InDetTrackingGeometry Builder : volume array creator
  Trk__TrackingVolumeArrayCreator=CompFactory.Trk.TrackingVolumeArrayCreator
  InDetTrackingVolumeArrayCreator                       = Trk__TrackingVolumeArrayCreator(name = namePrefix+'InDetTrackingVolumeArrayCreator'+nameSuffix)
  # add to ToolSvc
  result.addPublicTool(InDetTrackingVolumeArrayCreator)  

  # helpers for the InDetTrackingGeometry Builder : tracking volume helper for gluing
  Trk__TrackingVolumeHelper=CompFactory.Trk.TrackingVolumeHelper
  InDetTrackingVolumeHelper                             = Trk__TrackingVolumeHelper(name =namePrefix+'InDetTrackingVolumeHelper'+nameSuffix)
  # the material bins - assume defaults
  # add to ToolSvc
  result.addPublicTool(InDetTrackingVolumeHelper)  
  
  # helpers for the InDetTrackingGeometry Builder : cylinder volume creator
  Trk__CylinderVolumeCreator=CompFactory.Trk.CylinderVolumeCreator
  InDetCylinderVolumeCreator = Trk__CylinderVolumeCreator(name = namePrefix+'InDetCylinderVolumeCreator'+nameSuffix)
  # give it the layer array creator
  InDetCylinderVolumeCreator.LayerArrayCreator = InDetLayerArrayCreator
  # specifiy the binning, passive layers, entry layers - assume defaults
  # add to ToolSvc
  result.addPublicTool(InDetCylinderVolumeCreator)  

  if (namePrefix+name+nameSuffix).find('CondCond')>=0 :
      raise Exception('Invalid name composition %s + %s + %s ' % (namePrefix,name,nameSuffix))
  # the tracking geometry builder
  InDet__RobustTrackingGeometryBuilder=CompFactory.InDet.RobustTrackingGeometryBuilderCond
  return InDet__RobustTrackingGeometryBuilder(namePrefix+name+nameSuffix,
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
  
# Replaces https://gitlab.cern.ch/atlas/athena/blob/master/Calorimeter/CaloTrackingGeometry/python/ConfiguredCaloTrackingGeometryBuilder.py
def _getCaloTrackingGeometryBuilder(name, flags,result, envelopeDefinitionSvc, trackingVolumeHelper, namePrefix='',nameSuffix=''):
  # The following replaces LArCalorimeter/LArTrackingGeometry/python/ConfiguredLArVolumeBuilder.py
  from LArGeoAlgsNV.LArGMConfig import LArGMCfg
  result.merge(LArGMCfg(flags))
  LAr__LArVolumeBuilder=CompFactory.LAr.LArVolumeBuilder
  lArVolumeBuilder = LAr__LArVolumeBuilder(namePrefix+'LArVolumeBuilder'+nameSuffix,
                                           TrackingVolumeHelper = trackingVolumeHelper,)
  result.addPublicTool(lArVolumeBuilder)
  
  # The following replaces TileCalorimeter/TileTrackingGeometry/python/ConfiguredTileVolumeBuilder.py
  from TileGeoModel.TileGMConfig import TileGMCfg
  result.merge(TileGMCfg(flags))
  Tile__TileVolumeBuilder=CompFactory.Tile.TileVolumeBuilder
  tileVolumeBuilder = Tile__TileVolumeBuilder( namePrefix+'TileVolumeBuilder'+nameSuffix,
                                               TrackingVolumeHelper = trackingVolumeHelper,)
  result.addPublicTool(tileVolumeBuilder)
  
  Calo__CaloTrackingGeometryBuilder=CompFactory.Calo.CaloTrackingGeometryBuilderCond
  return Calo__CaloTrackingGeometryBuilder(namePrefix+name+nameSuffix,
                                           LArVolumeBuilder = lArVolumeBuilder,
                                           TileVolumeBuilder = tileVolumeBuilder,
                                           TrackingVolumeHelper = trackingVolumeHelper,
                                           EnvelopeDefinitionSvc = envelopeDefinitionSvc,
                                           EntryVolumeName = "InDet::Containers::EntryVolume",
                                           ExitVolumeName = "Calo::Container",
                                           GapLayerEnvelope=5.0
                                           )

# Originally this function would use was TrkDetFlags.MaterialSource() and TrkDetFlags.MaterialValidation(). For new configuration, (temporarily?) pass as parameters.
# https://gitlab.cern.ch/atlas/athena/blob/master/Tracking/TrkDetDescr/TrkDetDescrSvc/python/AtlasTrackingGeometrySvc.py#L112
def TrackingGeometryCondAlgCfg( flags , name = 'AtlasTrackingGeometryCondAlg', doMaterialValidation=False ) :
    """
    Sets up the Tracking Geometry Conditions Algorithm
    """
    namePrefix=''
    nameSuffix='Cond'
    result = ComponentAccumulator()
    atlas_tracking_geometry_name = 'AtlasTrackingGeometry'
    Trk__GeometryBuilder=CompFactory.Trk.GeometryBuilderCond
    atlas_geometry_builder = Trk__GeometryBuilder(name = 'AtlasGeometryBuilder',
                                                  WorldDimension=[],
                                                  WorldMaterialProperties=[], 
                                                  TrackingVolumeArrayCreator=CompFactory.Trk.TrackingVolumeArrayCreator(name="TrackingVolumeArrayCreator"), 
                                                  TrackingVolumeHelper = CompFactory.Trk.TrackingVolumeHelper(name="TrackingVolumeHelper") )

    envelopSvcCfg= EnvelopeDefSvcCfg(flags)
    atlas_env_def_service=envelopSvcCfg.getPrimary()
    result.merge(envelopSvcCfg)

    # Depending on the job configuration, setup the various detector builders, and add to atlas_geometry_builder
    if flags.Detector.GeometryID:
      # TODO Not sure how to handle TrkDetFlags, specifically ISF_FatrasCustomGeometry, XMLFastCustomGeometry, SLHC_Geometry
      # So, here we only setup the default InDet geometry builder!
      inDetTrackingGeometryBuilder = _getInDetTrackingGeometryBuilder(name ='InDetTrackingGeometryBuilder',
                                                                      flags=flags,
                                                                      result=result,
                                                                      envelopeDefinitionSvc=atlas_env_def_service,
                                                                      buildTrtStrawLayers=flags.Beam.Type=='cosmics',
                                                                      namePrefix=namePrefix,
                                                                      nameSuffix=nameSuffix)
      
      atlas_geometry_builder.InDetTrackingGeometryBuilder = inDetTrackingGeometryBuilder
      
    if flags.Detector.GeometryCalo:
      Trk__CylinderVolumeCreator=CompFactory.Trk.CylinderVolumeCreator
      caloVolumeCreator = Trk__CylinderVolumeCreator(namePrefix+"CaloVolumeCreator"+nameSuffix)
      result.addPublicTool(caloVolumeCreator)

      Trk__TrackingVolumeHelper=CompFactory.Trk.TrackingVolumeHelper
      trackingVolumeHelper = Trk__TrackingVolumeHelper(name=namePrefix+'TrackingVolumeHelper'+nameSuffix)
      result.addPublicTool(trackingVolumeHelper)

      caloTrackingGeometryBuilder = _getCaloTrackingGeometryBuilder(name ='CaloTrackingGeometryBuilder',
                                                                    flags=flags,
                                                                    result=result,
                                                                    envelopeDefinitionSvc=atlas_env_def_service,
                                                                    trackingVolumeHelper=trackingVolumeHelper,
                                                                    namePrefix=namePrefix,
                                                                    nameSuffix=nameSuffix)
      atlas_geometry_builder.CaloTrackingGeometryBuilder = caloTrackingGeometryBuilder

    if flags.Detector.GeometryMuon:
      # Copied from from MuonTrackingGeometry.ConfiguredMuonTrackingGeometry import MuonTrackingGeometryBuilder
      # Add the muon geometry model to the CA
      from MuonConfig.MuonGeometryConfig import MuonGeoModelCfg
      result.merge(MuonGeoModelCfg(flags))
      muonStationTypeBuilder=CompFactory.Muon.MuonStationTypeBuilder(name = 'MuonStationTypeBuilder')

      Muon__MuonStationBuilder=CompFactory.Muon.MuonStationBuilderCond
      muonStationBuilder= Muon__MuonStationBuilder(name = namePrefix+'MuonStationBuilder'+nameSuffix)
      muonStationBuilder.StationTypeBuilder = muonStationTypeBuilder

      Muon__MuonInertMaterialBuilder=CompFactory.Muon.MuonInertMaterialBuilderCond
      muonInertMaterialBuilder= Muon__MuonInertMaterialBuilder(name = namePrefix+'MuonInertMaterialBuilder'+nameSuffix)

      Muon__MuonTrackingGeometryBuilder=CompFactory.Muon.MuonTrackingGeometryBuilderCond
      muonTrackingGeometryBuilder= Muon__MuonTrackingGeometryBuilder(name = namePrefix+'MuonTrackingGeometryBuilder'+nameSuffix,
                                                                     EnvelopeDefinitionSvc=atlas_env_def_service,
                                                                     MuonStationBuilder = muonStationBuilder,
                                                                     InertMaterialBuilder=muonInertMaterialBuilder
                                                                     )
      muonTrackingGeometryBuilder.EntryVolumeName = 'MuonSpectrometerEntrance'
      muonTrackingGeometryBuilder.ExitVolumeName  = 'Muon::Containers::MuonSystem'
    
      atlas_geometry_builder.MuonTrackingGeometryBuilder = muonTrackingGeometryBuilder
    

    # Now set up processors
    atlas_geometry_processors=[]
    
    if flags.TrackingGeometry.MaterialSource == 'COOL':
       CoolDataBaseFolder = '/GLOBAL/TrackingGeo/LayerMaterialV2' # Was from TrkDetFlags.MaterialStoreGateKey()
       # the material provider
       Trk__LayerMaterialProvider=CompFactory.Trk.LayerMaterialProvider
       atlasMaterialProvider = Trk__LayerMaterialProvider('AtlasMaterialProvider', LayerMaterialMapName=CoolDataBaseFolder)
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

    condAlg = Trk__TrackingGeometryCondAlg( name, GeometryBuilder = atlas_geometry_builder,
                                    TrackingGeometryWriteKey = atlas_tracking_geometry_name,
                                    GeometryProcessors = PrivateToolHandleArray(atlas_geometry_processors))
    result.addCondAlgo(condAlg, primary = True)
    
    # Hack for Single Threaded Athena: manually move dependencies of SCT_DetectorElementCondAlg
    # and PixelDetectorElementCondAlg such that these are executed after their dependencies.

    if flags.Concurrency.NumThreads <= 0:
        condAlgs = result._conditionsAlgs
        dependencies = {"PixelAlignCondAlg",
                        "SCT_AlignCondAlg",
                        "TRTAlignCondAlg",
                        "MuonAlignmentCondAlg",
                        "MuonDetectorCondAlg",
                        "CondInputLoader"}
        prependList = list()
        appendList = list()
        for alg in condAlgs:
            prepend = False
            for name in dependencies:              
              if str(alg).startswith(name+"("):
                prependList.append(alg)
                prepend = True
            if not prepend:
              appendList.append(alg)
        prependList.extend(appendList)
        condAlgs = prependList
        result._conditionsAlgs = condAlgs

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
