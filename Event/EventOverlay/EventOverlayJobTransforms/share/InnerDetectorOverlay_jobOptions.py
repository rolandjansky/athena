include.block ( "EventOverlayJobTransforms/InnerDetectorOverlay_jobOptions.py" )

from Digitization.DigitizationFlags import jobproperties
from AthenaCommon.DetFlags import DetFlags
from AthenaCommon import CfgGetter
from OverlayCommonAlgs.OverlayFlags import overlayFlags

from AthenaCommon.Resilience import treatException,protectedInclude

if DetFlags.overlay.pixel_on() or DetFlags.overlay.SCT_on() or DetFlags.overlay.TRT_on():

    if overlayFlags.doBkg==True:
       from OverlayCommonAlgs.OverlayCommonAlgsConf import DeepCopyObjects
       job += DeepCopyObjects("BkgRdo1")
       job.BkgRdo1.InDetObjects = True

    include( "InDetEventAthenaPool/InDetEventAthenaPool_joboptions.py" ) # FIXME: is needed?

    jobproperties.Digitization.doInDetNoise=False

    #if readBS and isRealData:
    #   include( "InDetCosmicRecExample/InDetCosmicFlags_jobOptions.py" )

    if DetFlags.overlay.pixel_on():
        job += CfgGetter.getAlgorithm("PixelOverlayDigitization")

        if readBS and isRealData:
           job.InDetPixelRawDataProvider.RDOKey = "OriginalEvent_SG+PixelRDOs"
           #ServiceMgr.ByteStreamAddressProviderSvc.TypeNames += [ "PixelRDO_Container/PixelRDOs" ]
           #ServiceMgr.ByteStreamAddressProviderSvc.TypeNames += [ "Trk::PixelClusterContainer/PixelOnlineClusters" ]
        else:
            if not conddb.folderRequested('PIXEL/PixReco'):
                conddb.addFolder('PIXEL_OFL','/PIXEL/PixReco')

    if DetFlags.overlay.SCT_on():

        # Setup the ReadCalibChip folders and Svc
        if isRealData:
           #conddb.blockFolder("/SCT/DAQ/Calibration/ChipGain")
           #conddb.blockFolder("/SCT/DAQ/Calibration/ChipNoise")
           #conddb.addFolder("SCT_OFL","/SCT/DAQ/Calibration/ChipGain",forceMC=True)
           #conddb.addFolder("SCT_OFL","/SCT/DAQ/Calibration/ChipNoise",forceMC=True)
           conddb.addFolder("SCT_OFL","/SCT/DAQ/Calibration/ChipGain<tag>SctDaqCalibrationChipGain-Apr10-01</tag>",forceMC=True, className="CondAttrListCollection")
           conddb.addFolder("SCT_OFL","/SCT/DAQ/Calibration/ChipNoise<tag>SctDaqCalibrationChipNoise-Apr10-01</tag>",forceMC=True, className="CondAttrListCollection")

           #if not conddb.folderRequested('/SCT/DAQ/Calibration/ChipGain'):
           #   conddb.addFolderSplitOnline("SCT","/SCT/DAQ/Calibration/ChipGain","/SCT/DAQ/Calibration/ChipGain",forceMC=True)
           #if not conddb.folderRequested('/SCT/DAQ/Calibration/ChipNoise'):
           #   conddb.addFolderSplitOnline("SCT","/SCT/DAQ/Calibration/ChipNoise","/SCT/DAQ/Calibration/ChipNoise",forceMC=True)

        job += CfgGetter.getAlgorithm("SCT_OverlayDigitization")
        CfgGetter.getPublicTool("SCT_DigitizationTool").InputObjectName="SCT_Hits"
        if readBS and isRealData:
           #job.InDetSCTRawDataProvider.EvtStore = "OriginalEvent_SG"
           job.InDetSCTRawDataProvider.RDOKey = "OriginalEvent_SG+SCT_RDOs"
           job.InDetSCTRawDataProvider.LVL1IDKey = "OriginalEvent_SG+SCT_LVL1ID"
           job.InDetSCTRawDataProvider.BCIDKey = "OriginalEvent_SG+SCT_BCID"
           #ServiceMgr.ByteStreamAddressProviderSvc.TypeNames += [ "SCT_RDO_Container/SCT_RDOs" ]
           #ServiceMgr.ByteStreamAddressProviderSvc.TypeNames += [ "Trk::SCT_ClusterContainer/SCT_OnlineClusters" ]

    if DetFlags.overlay.TRT_on():
        if isRealData:
           conddb.blockFolder("/TRT/Cond/DigVers")
           #conddb.addFolderWithTag("TRT_OFL","/TRT/Cond/DigVers","TRTCondDigVers-Collisions-01",force=True,forceMC=True)
           conddb.addFolder("TRT_OFL","/TRT/Cond/DigVers",forceMC=True)

        job += CfgGetter.getAlgorithm("TRT_OverlayDigitization")

        if readBS and isRealData:
           job.InDetTRTRawDataProvider.EvtStore = "OriginalEvent_SG"
           job.InDetTRTRawDataProvider.RDOKey = "OriginalEvent_SG+TRT_RDOs"
           #ServiceMgr.ByteStreamAddressProviderSvc.TypeNames += [ "TRT_RDO_Container/TRT_RDOs" ]
  
           from TRT_ConditionsServices.TRT_ConditionsServicesConf import TRT_CalDbSvc
           InDetTRTCalDbSvc = TRT_CalDbSvc()
           ServiceMgr += InDetTRTCalDbSvc
          #from IOVDbSvc.CondDB import conddb
#           conddb.addFolder("TRT","/TRT/Calib/T0","<tag>TrtCalibt0-UPD2-FDR2-01</tag>")
#           conddb.addFolder("TRT","/TRT/Calib/RT","<tag>TrtCalibRt-UPD2-FDR2-01</tag>")
      #     conddb.addFolder("TRT","/TRT/Calib/T0","<tag>TrtCalibRt-HLT-UPD1-01</tag>")
       #    conddb.addFolder("TRT","/TRT/Calib/RT","<tag>TrtCalibT0-HLT-UPD1-01</tag>")
           conddb.addFolder("TRT_ONL","/TRT/Onl/ROD/Compress")

    if overlayFlags.doSignal==True:
       include ("EventOverlayJobTransforms/InDetMcSignal_jobOptions.py")

    job += CfgGetter.getAlgorithm("InDetOverlay")
    job += CfgGetter.getAlgorithm("InDetSDOOverlay")
