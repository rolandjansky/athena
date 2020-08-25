include.block ( "EventOverlayJobTransforms/CaloOverlay_jobOptions.py" )

from Digitization.DigitizationFlags import jobproperties
from AthenaCommon.DetFlags import DetFlags
from OverlayCommonAlgs.OverlayFlags import overlayFlags


if DetFlags.overlay.LAr_on() or DetFlags.overlay.Tile_on():

   jobproperties.Digitization.doCaloNoise=False

   if overlayFlags.isDataOverlay():
      include("LArConditionsCommon/LArIdMap_comm_jobOptions.py")
      include("LArConditionsCommon/LArConditionsCommon_comm_jobOptions.py")
   else:
      include("LArConditionsCommon/LArIdMap_MC_jobOptions.py")
      include("LArConditionsCommon/LArConditionsCommon_MC_jobOptions.py")

   include( "LArIdCnv/LArIdCnv_joboptions.py" )
   include( "CaloDetMgrDetDescrCnv/CaloDetMgrDetDescrCnv_joboptions.py" )

if DetFlags.overlay.LAr_on():

    # FIXME this is for doing Overlay with MC RDO + MC hits
    #   real data RDO will require some changes in the setup for proper db acces
    #include( "LArDetDescr/LArDetDescr_joboptions.py" )
    #include( "LArAthenaPool/LArAthenaPool_joboptions.py" )
    # We also need the conditions svc for MC constants:
    if overlayFlags.isDataOverlay():
       from LArROD.LArRODFlags import larRODFlags
       larRODFlags.keepDSPRaw = False
       from LArByteStream.LArByteStreamConf import LArRawDataReadingAlg
       try:
          #inhibit the reading of LArawChanels, they are created offline from the overlaid LArDigits:
          job.LArRawDataReadingAlg.LArRawChannelKey="" 
          #Configure the reading of the background digits from ByteStream
          if overlayFlags.isOverlayMT():
             job.LArRawDataReadingAlg.LArDigitKey="FREE"
          else:
             job.LArRawDataReadingAlg.LArDigitKey=overlayFlags.dataStore()+"+FREE"
       except AttributeError:
          #in case the LArRawDataReadingAlg was not set up by someone:
          if overlayFlags.isOverlayMT():
             job+=LArRawDataReadingAlg(LArRawChannelKey="",
                                       LArDigitKey="FREE")
          else:   
             job+=LArRawDataReadingAlg(LArRawChannelKey="",
                                       LArDigitKey=overlayFlags.dataStore()+"+FREE")
          pass

    from LArDigitization.LArDigitGetter import LArDigitGetter
    theLArDigitGetter = LArDigitGetter()


    if overlayFlags.isDataOverlay():
       from LArROD.LArRawChannelBuilderDefault import LArRawChannelBuilderDefault
       LArRawChannelBuilderDefault()
    else:
        job += CfgGetter.getAlgorithm("LArRawChannelBuilder", tryDefaultConfigurable=True)

    from LArROD.LArDigits import DefaultLArDigitThinner
    LArDigitThinner = DefaultLArDigitThinner('LArDigitThinner') # automatically added to topSequence

    #Adjust StoreGate keys in case of data-overlay:
    if overlayFlags.isDataOverlay():
       #Digits read from ByteStream:
       job.digitmaker1.LArPileUpTool.InputDigitContainer="FREE"

       #Output of the LArPileUpTool (set up by LArDigitGetter) is "LArDigitContainer_MC"
       #That's the input to the digit thinner and the LArRawChannelBuilder
       job.LArDigitThinner.InputContainerName = "LArDigitContainer_MC"
       job.LArRawChannelBuilderAlg.LArDigitKey = "LArDigitContainer_MC"



#----------------------------------------------------------------
if DetFlags.overlay.Tile_on():

    include( "TileIdCnv/TileIdCnv_jobOptions.py" )
    include( "TileConditions/TileConditions_jobOptions.py" )

    include( "TileSimAlgs/TileDigitization_jobOptions.py" )
    include( "TileL2Algs/TileL2Algs_jobOptions.py" )

    job.TileHitVecToCnt.DigitizationTool.RndmEvtOverlay = True
    job.TileHitVecToCnt.DigitizationTool.OnlyUseContainerName = not overlayFlags.isOverlayMT()
    theTileDigitsMaker.RndmEvtOverlay = True
    theTileDigitsMaker.InputTileDigitContainer = "TileDigitsCnt"
    if overlayFlags.isOverlayMT():
       theTileDigitsMaker.InputTileDigitContainer = overlayFlags.bkgPrefix() + "TileDigitsCnt"
       theTileDigitsMaker.OnlyUseContainerName = False
    if overlayFlags.isDataOverlay():
       theApp.Dlls += [ "TileByteStream"]

       if overlayFlags.isOverlayMT():
          ServiceMgr.ByteStreamAddressProviderSvc.TypeNames += [ "TileRawChannelContainer/" + overlayFlags.bkgPrefix() + "TileRawChannelCnt"]
          ServiceMgr.ByteStreamAddressProviderSvc.TypeNames += [ "TileDigitsContainer/" + overlayFlags.bkgPrefix() + "TileDigitsCnt"]
          ServiceMgr.ByteStreamAddressProviderSvc.TypeNames += [ "TileL2Container/" + overlayFlags.bkgPrefix() + "TileL2Cnt"]
          ServiceMgr.ByteStreamAddressProviderSvc.TypeNames += [ "TileRawChannelContainer/" + overlayFlags.bkgPrefix() + "MuRcvRawChCnt"]
          ServiceMgr.ByteStreamAddressProviderSvc.TypeNames += [ "TileDigitsContainer/" + overlayFlags.bkgPrefix() + "MuRcvDigitsCnt"]
       else:
          ServiceMgr.ByteStreamAddressProviderSvc.TypeNames += [ "TileRawChannelContainer/TileRawChannelCnt"]
          ServiceMgr.ByteStreamAddressProviderSvc.TypeNames += [ "TileDigitsContainer/TileDigitsCnt"]
          ServiceMgr.ByteStreamAddressProviderSvc.TypeNames += [ "TileL2Container/TileL2Cnt"]
          ServiceMgr.ByteStreamAddressProviderSvc.TypeNames += [ "TileRawChannelContainer/MuRcvRawChCnt"]
          ServiceMgr.ByteStreamAddressProviderSvc.TypeNames += [ "TileDigitsContainer/MuRcvDigitsCnt"]

#--------------------
