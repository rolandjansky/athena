include.block ( "EventOverlayJobTransforms/InnerDetectorOverlay_jobOptions.py" )

from Digitization.DigitizationFlags import digitizationFlags
from AthenaCommon.DetFlags import DetFlags
from AthenaCommon import CfgGetter
from OverlayCommonAlgs.OverlayFlags import overlayFlags


if DetFlags.overlay.pixel_on() or DetFlags.overlay.SCT_on() or DetFlags.overlay.TRT_on():

    digitizationFlags.doInDetNoise=False # FIXME THIS SHOULD BE SET EARLIER IN THE CONFIGURATION

    #if overlayFlags.isDataOverlay():
    #   include( "InDetCosmicRecExample/InDetCosmicFlags_jobOptions.py" )

    if DetFlags.overlay.pixel_on():
        job += CfgGetter.getAlgorithm("PixelOverlayDigitization")
        job += CfgGetter.getAlgorithm("PixelOverlay")
        if DetFlags.overlay.Truth_on():
            job += CfgGetter.getAlgorithm("PixelSDOOverlay")

        if overlayFlags.isDataOverlay():
            job.InDetPixelRawDataProvider.RDOKey = overlayFlags.dataStore()+"+PixelRDOs"
            #ServiceMgr.ByteStreamAddressProviderSvc.TypeNames += [ "PixelRDO_Container/PixelRDOs" ]
            #ServiceMgr.ByteStreamAddressProviderSvc.TypeNames += [ "Trk::PixelClusterContainer/PixelOnlineClusters" ]
        else:
            if not conddb.folderRequested('PIXEL/PixReco'):
                conddb.addFolder('PIXEL_OFL','/PIXEL/PixReco')

    if DetFlags.overlay.SCT_on():

        # Setup the ReadCalibChip folders and Svc
        if overlayFlags.isDataOverlay():
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
        job += CfgGetter.getAlgorithm("SCTOverlay")
        if DetFlags.overlay.Truth_on():
            job += CfgGetter.getAlgorithm("SCTSDOOverlay")

        if overlayFlags.isDataOverlay():
            job.InDetSCTRawDataProvider.RDOKey = overlayFlags.dataStore()+"+SCT_RDOs"
            job.InDetSCTRawDataProvider.LVL1IDKey = overlayFlags.dataStore()+"+SCT_LVL1ID"
            job.InDetSCTRawDataProvider.BCIDKey = overlayFlags.dataStore()+"+SCT_BCID"
            #ServiceMgr.ByteStreamAddressProviderSvc.TypeNames += [ "SCT_RDO_Container/SCT_RDOs" ]
            #ServiceMgr.ByteStreamAddressProviderSvc.TypeNames += [ "Trk::SCT_ClusterContainer/SCT_OnlineClusters" ]

    if DetFlags.overlay.TRT_on():
        if overlayFlags.isDataOverlay():
            conddb.blockFolder("/TRT/Cond/DigVers")
            #conddb.addFolderWithTag("TRT_OFL","/TRT/Cond/DigVers","TRTCondDigVers-Collisions-01",force=True,forceMC=True)
            conddb.addFolder("TRT_OFL","/TRT/Cond/DigVers",forceMC=True)

        from TRT_ElectronPidTools.TRT_ElectronPidToolsConf import InDet__TRT_LocalOccupancy
        TRT_LocalOccupancy = InDet__TRT_LocalOccupancy(name="TRT_LocalOccupancy", isTrigger= False )

        job += CfgGetter.getAlgorithm("TRT_OverlayDigitization")
        job += CfgGetter.getAlgorithm("TRTOverlay")
        if DetFlags.overlay.Truth_on():
            job += CfgGetter.getAlgorithm("TRTSDOOverlay")

        job.TRTOverlay.TRT_LocalOccupancyTool  = TRT_LocalOccupancy

        from InDetRecExample.InDetJobProperties import InDetFlags
        include("InDetRecExample/InDetRecConditionsAccess.py")
        job.InDetTRTStrawStatusSummarySvc  = InDetTRTStrawStatusSummarySvc 
        
        if overlayFlags.isDataOverlay():
            job.InDetTRTRawDataProvider.RDOKey = overlayFlags.dataStore()+"+TRT_RDOs"
            #ServiceMgr.ByteStreamAddressProviderSvc.TypeNames += [ "TRT_RDO_Container/TRT_RDOs" ]

            #from IOVDbSvc.CondDB import conddb
            #conddb.addFolder("TRT","/TRT/Calib/T0","<tag>TrtCalibt0-UPD2-FDR2-01</tag>")
            #conddb.addFolder("TRT","/TRT/Calib/RT","<tag>TrtCalibRt-UPD2-FDR2-01</tag>")
            #conddb.addFolder("TRT","/TRT/Calib/T0","<tag>TrtCalibRt-HLT-UPD1-01</tag>")
            #conddb.addFolder("TRT","/TRT/Calib/RT","<tag>TrtCalibT0-HLT-UPD1-01</tag>")
            conddb.addFolder("TRT_ONL","/TRT/Onl/ROD/Compress",className='CondAttrListCollection')
