# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import CfgMgr
# The earliest bunch crossing time for which interactions will be sent
# to the HGTD Digitization code.
def HGTD_FirstXing():
    return 0
# The latest bunch crossing time for which interactions will be sent
# to the HGTD Digitization code.
def HGTD_LastXing():
    return 0
# NOTE: related to 3BC mode?

######################################################################################
def getHGTD_SurfaceChargesGenerator(name="HGTD_SurfaceChargesGenerator", **kwargs):

    from HGTD_Digitization.HGTD_DigitizationConf import HGTD_SurfaceChargesGenerator

    from AthenaCommon import CfgMgr
    return CfgMgr.HGTD_SurfaceChargesGenerator(name, **kwargs)


######################################################################################
def getHGTD_FrontEndTool(name="HGTD_FrontEndTool", **kwargs):
    from Digitization.DigitizationFlags import digitizationFlags

    from HGTD_Digitization.HGTD_DigitizationConf import HGTD_FrontEndTool

    from AthenaCommon import CfgMgr
    return CfgMgr.HGTD_FrontEndTool(name, **kwargs)

######################################################################################

def commonHGTD_DigitizationConfig(name,**kwargs):

    from Digitization.DigitizationFlags import digitizationFlags

    kwargs.setdefault("InputCollecionName", "HGTD_Hits")

    kwargs.setdefault("RndmSvc", digitizationFlags.rndmSvc() )
    streamName = "HGTD_Digitization" # FIXME ideally random stream name would be configurable
    if not digitizationFlags.rndmSeedList.checkForExistingSeed(streamName):
        digitizationFlags.rndmSeedList.addSeed(streamName, 49261510, 105132394 )

    if digitizationFlags.doXingByXingPileUp():
        kwargs.setdefault("FirstXing", HGTD_FirstXing())
        kwargs.setdefault("LastXing",  HGTD_LastXing() )

    from AthenaCommon import CfgMgr
    return CfgMgr.HGTD_DigitizationTool(name,**kwargs)

######################################################################################
def getHGTD_DigitizationTool(name="HGTD_DigitizationTool", **kwargs):
    kwargs.setdefault("OutoutRDOContName", "HGTD_RDOs")
    kwargs.setdefault("OutputSDOName", "HGTD_SDO_Map")
    return commonHGTD_DigitizationConfig(name,**kwargs)


######################################################################################
## FIXME needs to go back in later
# def getHGTD_Amp(name="HGTD_Amp", **kwargs):
#     kwargs.setdefault("CrossFactor2sides", 0.1)
#     kwargs.setdefault("CrossFactorBack", 0.07)
#     kwargs.setdefault("PeakTime", 21)
#     kwargs.setdefault("deltaT", 1.0)
#     kwargs.setdefault("Tmin", -25.0)
#     kwargs.setdefault("Tmax", 150.0)
#     kwargs.setdefault("NbAverage", 0)
#     from HGTD_Digitization.HGTD_DigitizationConf import HGTD_Amp
#     return HGTD_Amp(name, **kwargs)


######################################################################################
def getHGTD_FrontEnd(name="HGTD_FrontEnd", **kwargs):
    from Digitization.DigitizationFlags import digitizationFlags

    from HGTD_Digitization.HGTD_DigitizationConf import HGTD_FrontEnd
    return HGTD_FrontEnd(name, **kwargs)
