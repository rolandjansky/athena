# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import CfgMgr

# The earliest bunch crossing time for which interactions will be sent
# to the ALFA Digitization code.
def ALFA_FirstXing():
    return 0 #Assume ALFA is only sensitive to the current bunch crossing, for the moment

######################################################################################

# The latest bunch crossing time for which interactions will be sent
# to the ALFA Digitization code.
def ALFA_LastXing():
    return 0 #Assume ALFA is only sensitive to the current bunch crossing, for the moment

######################################################################################

def getALFARange(name="ALFARange" , **kwargs):
    #this is the time of the xing in ns                                                                                
    kwargs.setdefault('FirstXing', ALFA_FirstXing() )
    kwargs.setdefault('LastXing',  ALFA_LastXing() )
    kwargs.setdefault('CacheRefreshFrequency', 1.0 ) #default 0 no dataproxy reset                                              
    kwargs.setdefault('ItemList', ["ALFA_HitCollection#ALFA_HitCollection",
                                   "ALFA_ODHitCollection#ALFA_ODHitCollection"] )
    return CfgMgr.PileUpXingFolder(name, **kwargs)

######################################################################################

def getALFA_PileUpTool(name="ALFA_PileUpTool",**kwargs):
    from Digitization.DigitizationFlags import digitizationFlags
    kwargs.setdefault("RndmSvc", digitizationFlags.rndmSvc.get_Value() )
    ALFARndmStream = "ALFARndEng"
    digitizationFlags.rndmSeedList.addSeed(ALFARndmStream, 3593, 130977)
    # Configure bunch-crossing envelope
    if digitizationFlags.doXingByXingPileUp():
        kwargs.setdefault("FirstXing", ALFA_FirstXing() )
        kwargs.setdefault("LastXing", ALFA_LastXing() )

    return CfgMgr.ALFA_PileUpTool(name, **kwargs)
