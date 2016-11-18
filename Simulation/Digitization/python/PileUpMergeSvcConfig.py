# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import CfgMgr
from AthenaCommon import CfgGetter
#FIXME no need for all these getPrivateTool calls.
def getPileUpMergeSvc(name="PileUpMergeSvc", **kwargs):
    from Digitization.DigitizationFlags import digitizationFlags
    if not digitizationFlags.doXingByXingPileUp(): # PileUpTool approach
        IntervalsList = []
        from AthenaCommon.DetFlags import DetFlags
        ## Truth information
        if DetFlags.pileup.Truth_on():
            IntervalsList += [ CfgGetter.getPrivateTool("TruthJetRange", checkType=True) ]
            if DetFlags.writeRDOPool.Muon_on(): #possibly this should be digitize.Muon_on()
                IntervalsList += [ CfgGetter.getPrivateTool("TrackRange", checkType=True) ]
            if DetFlags.writeRDOPool.Calo_on(): #possibly this should be digitize.Calo_on()
                IntervalsList += [ CfgGetter.getPrivateTool("CalibRange", checkType=True) ]
        ## Forward Detector Digitization
        if DetFlags.pileup.Lucid_on():
            IntervalsList += [ CfgGetter.getPrivateTool("LucidRange",     checkType=True) ]
        if DetFlags.pileup.ZDC_on():
            IntervalsList += [ CfgGetter.getPrivateTool("ZdcRange",       checkType=True) ]
        if DetFlags.pileup.ALFA_on():
            IntervalsList += [ CfgGetter.getPrivateTool("ALFARange",      checkType=True) ]
        if DetFlags.pileup.AFP_on():
            IntervalsList += [ CfgGetter.getPrivateTool("AFPRange",      checkType=True) ]
        ## Inner Detector Digitization
        if DetFlags.pileup.BCM_on():
            IntervalsList += [ CfgGetter.getPrivateTool("BCM_Range",      checkType=True) ]
        if DetFlags.pileup.pixel_on():
            if 'doFastPixelDigi' in digitizationFlags.experimentalDigi():
                IntervalsList += [ CfgGetter.getPrivateTool("FastPixelRange", checkType=True) ]
            else:
                IntervalsList += [ CfgGetter.getPrivateTool("PixelRange",     checkType=True) ]
        if DetFlags.pileup.SCT_on():
            if 'doFastSCT_Digi' in digitizationFlags.experimentalDigi():
                IntervalsList += [ CfgGetter.getPrivateTool("FastSCTRange",   checkType=True) ]
            else:
                IntervalsList += [ CfgGetter.getPrivateTool("SiliconRange",   checkType=True) ]
        if DetFlags.pileup.TRT_on():
            if 'doFastTRT_Digi' in digitizationFlags.experimentalDigi():
                IntervalsList += [ CfgGetter.getPrivateTool("FastTRTRange",   checkType=True) ]
            else:
                IntervalsList += [ CfgGetter.getPrivateTool("TRTRange",       checkType=True) ]
        ## Calo Digitization
        from LArDigitization.LArDigitizationConfig import isOverlay
        if DetFlags.pileup.LAr_on() or isOverlay():
            IntervalsList += [ CfgGetter.getPrivateTool("LArRangeEM",     checkType=True) ]
            IntervalsList += [ CfgGetter.getPrivateTool("LArRangeHEC",    checkType=True) ]
            IntervalsList += [ CfgGetter.getPrivateTool("LArRangeFCAL",   checkType=True) ]
        if hasattr(DetFlags.pileup, 'HGTD_on') and DetFlags.pileup.HGTD_on(): # No HGTD in Overlay samples (yet!)
            IntervalsList += [ CfgGetter.getPrivateTool("LArRangeHGTD",   checkType=True) ]
        if DetFlags.pileup.Tile_on():
            IntervalsList += [ CfgGetter.getPrivateTool("TileRange",      checkType=True) ]
        ## Muon System Digitization
        if DetFlags.pileup.CSC_on():
            IntervalsList += [ CfgGetter.getPrivateTool("CscRange",       checkType=True) ]
        if DetFlags.pileup.MDT_on():
            IntervalsList += [ CfgGetter.getPrivateTool("MdtRange",       checkType=True) ]
        if DetFlags.pileup.RPC_on():
            IntervalsList += [ CfgGetter.getPrivateTool("RpcRange",       checkType=True) ]
        if DetFlags.pileup.TGC_on():
            IntervalsList += [ CfgGetter.getPrivateTool("TgcRange",       checkType=True) ]
        if DetFlags.pileup.sTGC_on():
            IntervalsList += [ CfgGetter.getPrivateTool("sTgcRange",      checkType=True) ]
        if DetFlags.pileup.Micromegas_on():
            IntervalsList += [ CfgGetter.getPrivateTool("MmRange",        checkType=True) ]
        # RecoTimingObj
        if DetFlags.pileup.any_on():
            IntervalsList += [ CfgGetter.getPrivateTool("TimingObjRange", checkType=True) ]
        kwargs.setdefault("Intervals", IntervalsList)
    from PileUpTools.PileUpToolsConf import PileUpMergeSvc
    return PileUpMergeSvc(name, **kwargs)
