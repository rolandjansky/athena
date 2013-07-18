# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


# The earliest bunch crossing time for which interactions will be sent
# to the LUCID Digitization code.
def LUCID_FirstXing():
    return 0

# The latest bunch crossing time for which interactions will be sent
# to the LUCID Digitization code.
def LUCID_LastXing():
    return 0

######################################################################################

def getLucidRange(name="LucidRange" , **kwargs):
    #this is the time of the xing in ns                                                                                
    kwargs.setdefault('FirstXing', LUCID_FirstXing() ) #LUCID is only sensitive to the current bunch crossing.
    kwargs.setdefault('LastXing', LUCID_LastXing() ) #LUCID is only sensitive to the current bunch crossing.
    kwargs.setdefault('CacheRefreshFrequency', 1.0 ) #default 0 no dataproxy reset
    kwargs.setdefault('ItemList', ["LUCID_SimHitCollection#LucidSimHitsVector"] )
    from AthenaCommon import CfgMgr
    return CfgMgr.PileUpXingFolder(name, **kwargs)

######################################################################################

def getLUCID_PileUpTool(name="LUCID_PileUpTool",**kwargs):

    kwargs.setdefault('pmtSmearing', [0.317, 0.000, 0.292, 0.316, 0.208, 0.178, 0.204, 0.281, 0.233, 0.261, 0.223, 0.250, 0.254, 0.239, 0.202, 0.224,  1,  1,  1,  1,
                          0.268, 0.277, 0.297, 0.310, 0.203, 0.347, 0.269, 0.241, 0.234, 0.234, 0.277, 0.297, 0.225, 0.297, 0.238, 0.000,  1,  1,  1,  1] )
    kwargs.setdefault('pmtScaling', [1.010, 0.000, 0.991, 0.948, 1.152, 1.221, 1.160, 0.988, 1.092, 1.063, 1.143, 1.091, 1.109, 1.117, 1.185, 1.142,  1,  1,  1,  1,
                          1.023, 1.127, 1.043, 0.986, 1.148, 0.899, 0.898, 1.098, 1.115, 1.109, 1.127, 1.043, 1.085, 1.043, 1.063, 0.000,  1,  1,  1,  1] )
    kwargs.setdefault('gasScaling', [1.176, 0.000, 1.217, 1.101, 1.143, 1.105, 1.103, 1.144, 1.075, 1.069, 1.100, 1.208, 1.212, 1.125, 1.026, 1.037,  1,  1,  1,  1,
                          1.064, 0.956, 0.975, 0.938, 1.205, 1.095, 1.137, 1.222, 1.262, 1.160, 0.923, 0.969, 1.132, 0.969, 1.174, 0.000,  1,  1,  1,  1] )
    kwargs.setdefault('npeThreshold', [   17,    15,    16,    16,    18,    16,    16,    18,    17,    16,    16,    17,    19,    16,    16,    17, 15, 15, 15, 15,
                             17,    16,    16,    17,    17,    15,    16,    16,    17,    16,    15,    17,    17,    15,    16,    15, 15, 15, 15, 15] )

    from Digitization.DigitizationFlags import digitizationFlags
    kwargs.setdefault('RndmSvc', digitizationFlags.rndmSvc.get_Value() )

    LucidRndmStream = "LUCIDRndEng"

    digitizationFlags.rndmSeedList.addSeed(LucidRndmStream, 3591, 2309736)

    # Configure bunch-crossing envelope
    if digitizationFlags.doXingByXingPileUp():
        kwargs.setdefault("FirstXing", LUCID_FirstXing() )
        kwargs.setdefault("LastXing", LUCID_LastXing() )

    from AthenaCommon import CfgMgr
    return CfgMgr.LUCID_PileUpTool(name,**kwargs)

