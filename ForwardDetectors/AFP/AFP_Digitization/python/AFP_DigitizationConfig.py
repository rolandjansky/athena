# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import CfgMgr

# The earliest bunch crossing time for which interactions will be sent
# to the AFP Digitization code.
def AFP_FirstXing():
    return 0 #Assume AFP is only sensitive to the current bunch crossing, for the moment

######################################################################################

# The latest bunch crossing time for which interactions will be sent
# to the AFP Digitization code.
def AFP_LastXing():
    return 0 #Assume AFP is only sensitive to the current bunch crossing, for the moment

######################################################################################

def getAfpRange(name="AFPRange" , **kwargs):
    #this is the time of the xing in ns                                                                                
    kwargs.setdefault('FirstXing', AFP_FirstXing() )
    kwargs.setdefault('LastXing',  AFP_LastXing() )
    kwargs.setdefault('CacheRefreshFrequency', 1.0 ) #default 0 no dataproxy reset                                              
    kwargs.setdefault('ItemList', ["AFP_TDSimHitCollection_p1#AFP_TDSimHitCollection", 
                                   "AFP_SIDSimHitCollection_p1#AFP_SIDSimHitCollection"] )
    return CfgMgr.PileUpXingFolder(name, **kwargs)

######################################################################################

def getAFP_PileUpTool(name="AFP_PileUpTool",**kwargs):
    from Digitization.DigitizationFlags import digitizationFlags
    # Configure bunch-crossing envelope
    if digitizationFlags.doXingByXingPileUp():
        kwargs.setdefault("FirstXing", AFP_FirstXing() )
        kwargs.setdefault("LastXing", AFP_LastXing() )

    return CfgMgr.AFP_PileUpTool(name, **kwargs)
