# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration


# The earliest bunch crossing time for which interactions will be sent
# to the BCM Digitization code.
def BCM_FirstXing():
    return -25

# The latest bunch crossing time for which interactions will be sent
# to the BCM Digitization code.
def BCM_LastXing():
    return 0

def BCM_DigitizationTool(name="BCM_DigitizationTool",**kwargs):

    from Digitization.DigitizationFlags import digitizationFlags
    kwargs.setdefault("HitCollName", "BCMHits")

    if not digitizationFlags.doInDetNoise.get_Value():
        kwargs.setdefault("ModNoise", [ 0., 0., 0., 0., 0., 0., 0., 0. ])
    else:
        kwargs.setdefault("ModNoise", [ 90.82, 90.82, 90.82, 90.82, 90.82, 90.82, 90.82, 90.82 ])
    kwargs.setdefault("ModSignal", [ 450.0, 450.0, 450.0, 450.0, 450.0, 450.0, 450.0, 450.0 ])
    kwargs.setdefault("NinoThr", [ 330.0, 330.0, 330.0, 330.0, 330.0, 330.0, 330.0, 330.0 ])
    kwargs.setdefault("EffDistanceParam", 4.0)
    kwargs.setdefault("EffSharpnessParam", 0.11)
    kwargs.setdefault("TimeDelay", 9.0)
    
    kwargs.setdefault("MIPDeposit", 0.33) # BCM with diamond
    #kwargs.setdefault("MIPDeposit", 0.25) # BCM with graphite
 
    if digitizationFlags.doXingByXingPileUp():
        kwargs.setdefault("FirstXing", BCM_FirstXing() )
        kwargs.setdefault("LastXing",  BCM_LastXing()  ) 

    kwargs.setdefault("OutputRDOKey", "BCM_RDOs")
    kwargs.setdefault("OutputSDOKey", "BCM_SDO_Map")

    from AthenaCommon import CfgMgr
    return CfgMgr.BCM_DigitizationTool(name,**kwargs)


######################################################################################

def getBCM_Range(name="BCM_Range" , **kwargs):
    #this is the time of the xing in ns 
    kwargs.setdefault('FirstXing', BCM_FirstXing() )
    kwargs.setdefault('LastXing',  BCM_LastXing()  )
    kwargs.setdefault('CacheRefreshFrequency', 1.0 ) #default 0 no dataproxy reset
    kwargs.setdefault('ItemList', ["SiHitCollection#BCMHits"] )
    from AthenaCommon import CfgMgr
    return CfgMgr.PileUpXingFolder(name, **kwargs)

def BCM_OverlayDigitizationTool(name="BCM_OverlayDigitizationTool",**kwargs):
    from OverlayCommonAlgs.OverlayFlags import overlayFlags
    kwargs.setdefault("EvtStore", overlayFlags.evtStore())
    return BCM_DigitizationTool(name,**kwargs)

def BCM_OverlayDigitization(name="BCM_OverlayDigitization",**kwargs):
     kwargs.setdefault("DigitizationTool", "BCM_OverlayDigitizationTool")
     from AthenaCommon import CfgMgr
     return CfgMgr.BCM_Digitization(name,**kwargs)
