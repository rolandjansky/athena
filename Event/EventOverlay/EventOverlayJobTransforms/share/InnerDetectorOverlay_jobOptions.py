include.block ( "EventOverlayJobTransforms/InnerDetectorOverlay_jobOptions.py" )

from Digitization.DigitizationFlags import jobproperties
from OverlayCommonAlgs.OverlayFlags import OverlayFlags
from AthenaCommon import CfgMgr

from AthenaCommon.Resilience import treatException,protectedInclude

if OverlayFlags.doPixel() or OverlayFlags.doSCT() or OverlayFlags.doTRT():

    if OverlayFlags.doBkg():
       from OverlayCommonAlgs.OverlayCommonAlgsConf import DeepCopyObjects
       job += DeepCopyObjects("BkgRdo1")
       job.BkgRdo1.InDetObjects = True

    include( "InDetEventAthenaPool/InDetEventAthenaPool_joboptions.py" ) # FIXME: is needed?

    from InDetOverlay.InDetOverlayConf import InDetOverlay
    indetovl = InDetOverlay()

    jobproperties.Digitization.doInDetNoise=False

    #if readBS and isRealData:
    #   include( "InDetCosmicRecExample/InDetCosmicFlags_jobOptions.py" )

    if OverlayFlags.doPixel():
        protectedInclude( "PixelDigitization/PixelDigitization_jobOptions.py" )
        from AthenaCommon import CfgGetter
        CfgGetter.getPublicTool("PixelDigitizationTool",checkType=True).EvtStore = "BkgEvent_0_SG"
        indetovl.do_Pixel = True
        if readBS and isRealData:
           job.InDetPixelRawDataProvider.EvtStore = "OriginalEvent_SG"
           ServiceMgr.ByteStreamAddressProviderSvc.TypeNames += [ "PixelRDO_Container/PixelRDOs" ]
           #ServiceMgr.ByteStreamAddressProviderSvc.TypeNames += [ "Trk::PixelClusterContainer/PixelOnlineClusters" ]
    else:
        indetovl.do_Pixel = False

    if OverlayFlags.doSCT():

        # Setup the ReadCalibChip folders and Svc
        if isRealData:
           #conddb.blockFolder("/SCT/DAQ/Calibration/ChipGain")
           #conddb.blockFolder("/SCT/DAQ/Calibration/ChipNoise")
           #conddb.addFolder("SCT_OFL","/SCT/DAQ/Calibration/ChipGain",forceMC=True)
           #conddb.addFolder("SCT_OFL","/SCT/DAQ/Calibration/ChipNoise",forceMC=True)
           conddb.addFolder("SCT_OFL","/SCT/DAQ/Calibration/ChipGain<tag>SctDaqCalibrationChipGain-Apr10-01</tag>",forceMC=True)
           conddb.addFolder("SCT_OFL","/SCT/DAQ/Calibration/ChipNoise<tag>SctDaqCalibrationChipNoise-Apr10-01</tag>",forceMC=True)

           #if not conddb.folderRequested('/SCT/DAQ/Calibration/ChipGain'):
           #   conddb.addFolderSplitOnline("SCT","/SCT/DAQ/Calibration/ChipGain","/SCT/DAQ/Calibration/ChipGain",forceMC=True)
           #if not conddb.folderRequested('/SCT/DAQ/Calibration/ChipNoise'):
           #   conddb.addFolderSplitOnline("SCT","/SCT/DAQ/Calibration/ChipNoise","/SCT/DAQ/Calibration/ChipNoise",forceMC=True)

        # Dynamic configuration of SCT RDO type
        # This algorithm must be executed before SCT_Digitization
        from InDetOverlay.InDetOverlayConf import DynConfSCT
        job += DynConfSCT()


        protectedInclude( "SCT_Digitization/SCT_Digitization_jobOptions.py" )
        if hasattr(job.SCT_Digitization, "DigitizationTool"):
            from AthenaCommon import CfgGetter
            CfgGetter.getPublicTool("SCT_DigitizationTool",checkType=True).EvtStore = "BkgEvent_0_SG"
        else:
            job.SCT_Digitization.EvtStore  = "BkgEvent_0_SG"
        indetovl.do_SCT = True
        if readBS and isRealData:
           job.InDetSCTRawDataProvider.EvtStore = "OriginalEvent_SG"
           ServiceMgr.ByteStreamAddressProviderSvc.TypeNames += [ "SCT_RDO_Container/SCT_RDOs" ]
           #ServiceMgr.ByteStreamAddressProviderSvc.TypeNames += [ "Trk::SCT_ClusterContainer/SCT_OnlineClusters" ]
    else:
        indetovl.do_SCT = False

    if OverlayFlags.doTRT():
        if isRealData:
           conddb.blockFolder("/TRT/Cond/DigVers")
           #conddb.addFolderWithTag("TRT_OFL","/TRT/Cond/DigVers","TRTCondDigVers-Collisions-01",force=True,forceMC=True)
           conddb.addFolder("TRT_OFL","/TRT/Cond/DigVers",forceMC=True)
        protectedInclude( "TRT_Digitization/TRT_Digitization_jobOptions.py" )
        if hasattr(job.TRTDigitization, "DigitizationTool"):
            job.TRTDigitization.DigitizationTool.EvtStore = "BkgEvent_0_SG"
        else:
            job.TRTDigitization.EvtStore   = "BkgEvent_0_SG"
        ### AG: FIXME: support old TRT digi for now.  The following block should be removed.
        try:
            job.TRTDigitization.StoreName   = "BkgEvent_0_SG"
            log.warning("TRTDigitization has ambiguous event store setup: two StoreGate handles!")
        except:
            log.info("InnerDetectorOverlay_jobOptions.py: caught an exception when trying to configure TRTDigitization using pre-AthAlgorithm StoreName. Time to remove this oboslete jO code.")

        indetovl.do_TRT = True
        if readBS and isRealData:
           job.InDetTRTRawDataProvider.EvtStore = "OriginalEvent_SG"
           ServiceMgr.ByteStreamAddressProviderSvc.TypeNames += [ "TRT_RDO_Container/TRT_RDOs" ]
           #ServiceMgr.ByteStreamAddressProviderSvc.TypeNames += [ "Trk::TRT_DriftCircleContainer/TRT_DriftCircle" ]
           if hasattr(job.TRTDigitization, "DigitizationTool"):
               job.TRTDigitization.DigitizationTool.Override_getT0FromData = 1
           else:
               job.TRTDigitization.Override_getT0FromData = 1

           from TRT_ConditionsServices.TRT_ConditionsServicesConf import TRT_CalDbSvc
           InDetTRTCalDbSvc = TRT_CalDbSvc()
           ServiceMgr += InDetTRTCalDbSvc
          #from IOVDbSvc.CondDB import conddb
#           conddb.addFolder("TRT","/TRT/Calib/T0","<tag>TrtCalibt0-UPD2-FDR2-01</tag>")
#           conddb.addFolder("TRT","/TRT/Calib/RT","<tag>TrtCalibRt-UPD2-FDR2-01</tag>")
           conddb.addFolder("TRT","/TRT/Calib/T0","<tag>TrtCalibRt-HLT-UPD1-01</tag>")
           conddb.addFolder("TRT","/TRT/Calib/RT","<tag>TrtCalibT0-HLT-UPD1-01</tag>")
           conddb.addFolder("TRT_ONL","/TRT/Onl/ROD/Compress")
    else:
        indetovl.do_TRT = False

    if OverlayFlags.doSignal():
       include ("EventOverlayJobTransforms/InDetMcSignal_jobOptions.py")

    job += indetovl
