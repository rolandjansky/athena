include.block ( "EventOverlayJobTransforms/TileMcSignal_jobOptions.py" )

from AthenaCommon.AppMgr import ToolSvc
from AthenaCommon.GlobalFlags import globalflags
from AthenaCommon import CfgGetter

if DetFlags.overlay.Tile_on():
#copy stuff back into the MC before persstency
   from OverlayCommonAlgs.OverlayCommonAlgsConf import RemoveObjects
   job += RemoveObjects("RemoveTileOldMC")
   if globalflags.DataSource()=='data':
      job.RemoveTileOldMC.RemoveTileMC=True

   from TileRecUtils.TileRecFlags import jobproperties
   job += CfgGetter.getAlgorithm("TileHitVecToCnt/tilehitvect", tryDefaultConfigurable=True)
   tileHitVecToCnt=job.tilehitvect.DigitizationTool
   tileHitVecToCnt.EvtStore = "BkgEvent_0_SG"
   tileHitVecToCnt.RndmEvtOverlay = False
   tileHitVecToCnt.RndmSvc = job.TileHitVecToCnt.DigitizationTool.RndmSvc
   tileHitVecToCnt.TileHitVectors=job.TileHitVecToCnt.DigitizationTool.TileHitVectors
   tileHitVecToCnt.TileInfoName=job.TileHitVecToCnt.DigitizationTool.TileInfoName
   tileHitVecToCnt.TileHitContainer=job.TileHitVecToCnt.DigitizationTool.TileHitContainer
   from TileSimAlgs.TileSimAlgsConf import TileDigitsMaker
   theTileDigits=TileDigitsMaker("tiledigitmaker2")
   theTileDigits.EvtStore = "BkgEvent_0_SG"
   theTileDigits.TileHitContainer=job.TileDigitsMaker.TileHitContainer
   theTileDigits.TileInfoName=job.TileDigitsMaker.TileInfoName
   theTileDigits.IntegerDigits=job.TileDigitsMaker.IntegerDigits
   theTileDigits.CalibrationRun=job.TileDigitsMaker.CalibrationRun
   theTileDigits.TileDigitsContainer=job.TileDigitsMaker.TileDigitsContainer
   theTileDigits.RndmSvc = job.TileDigitsMaker.RndmSvc
   theTileDigits.UseCoolPulseShapes=job.TileDigitsMaker.UseCoolPulseShapes
   theTileDigits.MaskBadChannels=job.TileDigitsMaker.MaskBadChannels
   theTileDigits.RndmEvtOverlay=False
   job += theTileDigits
   from TileRecUtils.TileRecUtilsConf import TileRawChannelMaker
   newTileRawChannelMaker=TileRawChannelMaker("newTileRawChannelMaker")
   newTileRawChannelMaker.EvtStore = "BkgEvent_0_SG"
   newTileRawChannelMaker.TileRawChannelBuilder = []
   job += newTileRawChannelMaker

   from TileRecUtils.TileRecUtilsConf import TileBeamInfoProvider
   newTileBeamInfoProvider = TileBeamInfoProvider("newTileBeamInfoProvider")
   newTileBeamInfoProvider.TileBeamElemContainer = ToolSvc.TileBeamInfoProvider.TileBeamElemContainer
   newTileBeamInfoProvider.TileDigitsContainer = ToolSvc.TileBeamInfoProvider.TileDigitsContainer
   newTileBeamInfoProvider.TileRawChannelContainer = ToolSvc.TileBeamInfoProvider.TileRawChannelContainer
   newTileBeamInfoProvider.EvtStore = "BkgEvent_0_SG"
   ToolSvc += newTileBeamInfoProvider
  
   if jobproperties.TileRecFlags.doTileManyAmps():
      from TileRecUtils.TileRecUtilsConf import TileRawChannelBuilderManyAmps
      newTileRawChannelBuilderManyAmps= TileRawChannelBuilderManyAmps("newTileRawChannelBuilderManyAmps")
      newTileRawChannelBuilderManyAmps.TileRawChannelContainer = ToolSvc.TileRawChannelBuilderManyAmps.TileRawChannelContainer
      newTileRawChannelBuilderManyAmps.RunType = ToolSvc.TileRawChannelBuilderManyAmps.RunType
      newTileRawChannelBuilderManyAmps.calibrateEnergy = ToolSvc.TileRawChannelBuilderManyAmps.calibrateEnergy
      newTileRawChannelBuilderManyAmps.correctTime     =   ToolSvc.TileRawChannelBuilderManyAmps.correctTime
      newTileRawChannelBuilderManyAmps.EvtStore = "BkgEvent_0_SG"
      newTileRawChannelBuilderManyAmps.BeamInfo = ToolSvc.newTileBeamInfoProvider
      ToolSvc += newTileRawChannelBuilderManyAmps
      job.newTileRawChannelMaker.TileRawChannelBuilder += [ ToolSvc.newTileRawChannelBuilderManyAmps ]

   if jobproperties.TileRecFlags.doTileFlat():
      from TileRecUtils.TileRecUtilsConf import TileRawChannelBuilderFlatFilter
      newTileRawChannelBuilderFlatFilter= TileRawChannelBuilderFlatFilter("newTileRawChannelBuilderFlatFilter")
      newTileRawChannelBuilderFlatFilter.TileRawChannelContainer = ToolSvc.TileRawChannelBuilderFlatFilter.TileRawChannelContainer
      newTileRawChannelBuilderFlatFilter.RunType = ToolSvc.TileRawChannelBuilderFlatFilter.RunType
      newTileRawChannelBuilderFlatFilter.calibrateEnergy = ToolSvc.TileRawChannelBuilderFlatFilter.calibrateEnergy
      newTileRawChannelBuilderFlatFilter.correctTime     = ToolSvc.TileRawChannelBuilderFlatFilter.correctTime    
      newTileRawChannelBuilderFlatFilter.EvtStore = "BkgEvent_0_SG"
      newTileRawChannelBuilderFlatFilter.BeamInfo = ToolSvc.newTileBeamInfoProvider
      ToolSvc += newTileRawChannelBuilderFlatFilter
      job.newTileRawChannelMaker.TileRawChannelBuilder += [ ToolSvc.newTileRawChannelBuilderFlatFilter ]

   if jobproperties.TileRecFlags.doTileFit():
      from TileRecUtils.TileRecUtilsConf import TileRawChannelBuilderFitFilter
      newTileRawChannelBuilderFitFilter= TileRawChannelBuilderFitFilter("newTileRawChannelBuilderFitFilter")
      newTileRawChannelBuilderFitFilter.TileRawChannelContainer =ToolSvc.TileRawChannelBuilderFitFilter.TileRawChannelContainer 
      newTileRawChannelBuilderFitFilter.RunType = ToolSvc.TileRawChannelBuilderFitFilter.RunType
      newTileRawChannelBuilderFitFilter.calibrateEnergy = ToolSvc.TileRawChannelBuilderFitFilter.calibrateEnergy
      newTileRawChannelBuilderFitFilter.correctTime     = ToolSvc.TileRawChannelBuilderFitFilter.correctTime
      newTileRawChannelBuilderFitFilter.EvtStore = "BkgEvent_0_SG"
      newTileRawChannelBuilderFitFilter.BeamInfo = ToolSvc.newTileBeamInfoProvider
      ToolSvc += newTileRawChannelBuilderFitFilter
      job.newTileRawChannelMaker.TileRawChannelBuilder += [ ToolSvc.newTileRawChannelBuilderFitFilter ] 

   if jobproperties.TileRecFlags.doTileFitCool():
      from TileRecUtils.TileRecUtilsConf import TileRawChannelBuilderFitFilterCool
      TileRawChannelBuilderFitFilterCool= TileRawChannelBuilderFitFilterCool("newTileRawChannelBuilderFitFilterCool")
      newTileRawChannelBuilderFitFilterCool.TileRawChannelContainer = ToolSvc.TileRawChannelBuilderFitFilterCool.TileRawChannelContainer
      newTileRawChannelBuilderFitFilterCool.RunType = ToolSvc.TileRawChannelBuilderFitFilterCool.RunType
      newTileRawChannelBuilderFitFilterCool.calibrateEnergy =  ToolSvc.TileRawChannelBuilderFitFilterCool.calibrateEnergy
      newTileRawChannelBuilderFitFilterCool.correctTime     =  ToolSvc.TileRawChannelBuilderFitFilterCool.correctTime    
      newTileRawChannelBuilderFitFilterCool.EvtStore = "BkgEvent_0_SG"
      newTileRawChannelBuilderFitFilterCool.BeamInfo = ToolSvc.newTileBeamInfoProvider
      ToolSvc += newTileRawChannelBuilderFitFilterCool
      job.newTileRawChannelMaker.TileRawChannelBuilder += [ ToolSvc.newTileRawChannelBuilderFitFilterCool ]

   if jobproperties.TileRecFlags.doTileOpt():
      from TileRecUtils.TileRecUtilsConf import TileRawChannelBuilderOptFilter
      newTileRawChannelBuilderOptFilter= TileRawChannelBuilderOptFilter("newTileRawChannelBuilderOptFilter")
      newTileRawChannelBuilderOptFilter.TileRawChannelContainer = ToolSvc.TileRawChannelBuilderOptFilter.TileRawChannelContainer
      newTileRawChannelBuilderOptFilter.RunType = ToolSvc.TileRawChannelBuilderOptFilter.RunType
      newTileRawChannelBuilderOptFilter.calibrateEnergy = ToolSvc.TileRawChannelBuilderOptFilter.calibrateEnergy
      newTileRawChannelBuilderOptFilter.correctTime     = ToolSvc.TileRawChannelBuilderOptFilter.correctTime
      newTileRawChannelBuilderOptFilter.OF2 = ToolSvc.TileRawChannelBuilderOptFilter.OF2
      newTileRawChannelBuilderOptFilter.PedestalMode = ToolSvc.TileRawChannelBuilderOptFilter.PedestalMode
      newTileRawChannelBuilderOptFilter.MaxIterations = ToolSvc.TileRawChannelBuilderOptFilter.MaxIterations
      newTileRawChannelBuilderOptFilter.Minus1Iteration = ToolSvc.TileRawChannelBuilderOptFilter.Minus1Iteration
      newTileRawChannelBuilderOptFilter.AmplitudeCorrection = ToolSvc.TileRawChannelBuilderOptFilter.AmplitudeCorrection 
      newTileRawChannelBuilderOptFilter.EvtStore = "BkgEvent_0_SG"
      newTileRawChannelBuilderOptFilter.BeamInfo = ToolSvc.newTileBeamInfoProvider
      ToolSvc += newTileRawChannelBuilderOptFilter
      job.newTileRawChannelMaker.TileRawChannelBuilder += [ ToolSvc.newTileRawChannelBuilderOptFilter ]

   if jobproperties.TileRecFlags.doTileOpt2():
      ToolSvc +=  ToolSvc.TileRawChannelBuilderOpt2Filter.clone("newTileRawChannelBuilderOpt2Filter")
      ToolSvc.newTileRawChannelBuilderOpt2Filter.EvtStore="BkgEvent_0_SG"
      job.newTileRawChannelMaker.TileRawChannelBuilder += [ ToolSvc.newTileRawChannelBuilderOpt2Filter ]

   if jobproperties.TileRecFlags.doTileOptATLAS():
      ToolSvc +=  ToolSvc.TileRawChannelBuilderOptATLAS.clone("newTileRawChannelBuilderOptATLAS")
      ToolSvc.newTileRawChannelBuilderOptATLAS.EvtStore="BkgEvent_0_SG"
      job.newTileRawChannelMaker.TileRawChannelBuilder += [ ToolSvc.newTileRawChannelBuilderOptATLAS ]

