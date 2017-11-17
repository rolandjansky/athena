include.block ( "EventOverlayJobTransforms/LArMcSignal_jobOptions.py" )

from AthenaCommon.GlobalFlags import globalflags
from LArROD.LArRODFlags import larRODFlags

# calorimeter
if DetFlags.overlay.LAr_on():

   # Remove some objects from MC event store before re-running digitization
   from OverlayCommonAlgs.OverlayCommonAlgsConf import RemoveObjects
   job += RemoveObjects("RemoveLArOldMC")
   if globalflags.DataSource()=='data':
      job.RemoveLArOldMC.RemoveLArMC=True

   from LArDigitization.LArDigitizationConf import LArDigitMaker
   theLArDigits = LArDigitMaker("digitmaker2")
   theLArDigits.EvtStore = "BkgEvent_0_SG"
   #theLArDigits.DigitContainer = job.digitmaker1.DigitContainer
   #theLArDigits.RandomDigitContainer = job.digitmaker1.RandomDigitContainer
   #theLArDigits.RndmEvtOverlay = False
   #theLArDigits.NoiseOnOff = False
   #theLArDigits.NoiseInEMB =  job.digitmaker1.NoiseInEMB
   #theLArDigits.NoiseInEMEC = job.digitmaker1.NoiseInEMEC
   #theLArDigits.NoiseInHEC = job.digitmaker1.NoiseInHEC
   #theLArDigits.NoiseInFCAL = job.digitmaker1.NoiseInFCAL
   #theLArDigits.RecordMap = False

   #job.digitmaker1.LArPileUpTool.NoiseOnOff = True #ACH
   #job.digitmaker1.OutputLevel=DEBUG #ACH
   #job.digitmaker1.LArPileUpTool.OutputLevel=DEBUG #ACH
   #MessageSvc.debugLimit = 10000 #ACH

   from LArDigitization.LArDigitizationConf import LArPileUpTool
   theLArPileUpTool = LArPileUpTool("LArPileUpTool2")
   theLArPileUpTool.EvtStore = "BkgEvent_0_SG"
   theLArPileUpTool.DigitContainer = job.digitmaker1.LArPileUpTool.DigitContainer
   theLArPileUpTool.RndmEvtOverlay = False
   theLArPileUpTool.NoiseOnOff = False
   theLArPileUpTool.RecordMap = False
   
   if isRealData:
      theLArPileUpTool.PedestalKey = job.digitmaker1.LArPileUpTool.PedestalKey

   theLArPileUpTool.ADC2MeVTool = job.digitmaker1.LArPileUpTool.ADC2MeVTool
   theLArPileUpTool.AutoCorrNoiseTool = job.digitmaker1.LArPileUpTool.AutoCorrNoiseTool
   theLArPileUpTool.useLArFloat = job.digitmaker1.LArPileUpTool.useLArFloat
   theLArPileUpTool.MaskingTool = job.digitmaker1.LArPileUpTool.MaskingTool
   theLArPileUpTool.BadChannelTool = job.digitmaker1.LArPileUpTool.BadChannelTool
   theLArDigits.LArPileUpTool = theLArPileUpTool

   job += theLArDigits


   from LArROD.LArRODConf import LArRawChannelBuilder
   newLArRawChannelBuilder =  LArRawChannelBuilder( "newLArRawChannelBuilder" )
   newLArRawChannelBuilder.DataLocation = job.LArRawChannelBuilder.DataLocation
   newLArRawChannelBuilder.ADC2MeVTool = ToolSvc.LArADC2MeVToolDefault
   if globalflags.DataSource()=='data' or larRODFlags.forceIter() :
      newLArRawChannelBuilder.UseOFCTool= False
      newLArRawChannelBuilder.PhaseInversion=True
      newLArRawChannelBuilder.LArRawChannelContainerName = "LArRawChannels"
      newLArRawChannelBuilder.PedestalKey='Pedestal'
   else :
      from LArRecUtils.LArOFCToolDefault import LArOFCToolDefault
      theOFCTool = LArOFCToolDefault()
      ToolSvc += theOFCTool
      newLArRawChannelBuilder.OFCTool =  theOFCTool
      newLArRawChannelBuilder.LArRawChannelContainerName = job.LArRawChannelBuilder.LArRawChannelContainerName
      newLArRawChannelBuilder.UseOFCTool= True
   newLArRawChannelBuilder.EvtStore = "BkgEvent_0_SG"
   newLArRawChannelBuilder.OutputLevel = DEBUG
   job += newLArRawChannelBuilder

   from LArROD.LArDigits import DefaultLArDigitThinner
   newLArDigitThinner = DefaultLArDigitThinner('newLArDigitThinner') # automatically added to topSequence
   job.newLArDigitThinner.EvtStore = "BkgEvent_0_SG"

