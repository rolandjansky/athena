# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

def getForwardTransportSvc(name="ForwardTransportSvc", **kwargs):
    from ForwardTransportSvc.ForwardTransportFlags import forwardTransportFlags
    forwardTransportFlags.SetTwissFilePaths()
    kwargs.setdefault("TwissFile1", forwardTransportFlags.twissFilePath1.get_Value())
    kwargs.setdefault("TwissFile2", forwardTransportFlags.twissFilePath2.get_Value())
    kwargs.setdefault("PositionC1", 149)
    kwargs.setdefault("PositionC2", 184)
    kwargs.setdefault("ApertureC1", 999)
    kwargs.setdefault("ApertureC2", 999)
    detFlag = forwardTransportFlags.detectorFlag.get_Value()
    if   detFlag == "ALFA": return getALFAForwardTransportSvc(name, **kwargs)
    elif detFlag == "ZDC":  return getZDCForwardTransportSvc (name, **kwargs)
    else:                   print ("Uknown operation for detectorFlag: ", detFlag)

def getALFAForwardTransportSvc(name="ForwardTransportSvc", **kwargs):
    print ("getALFAForwardTransportSvc")
    kwargs.setdefault("EndMarker",     236.888)
    kwargs.setdefault("TransportFlag", 1)
    kwargs.setdefault("EtaCut",        7.5)
    kwargs.setdefault("XiCut",         0.8)
    from AthenaCommon import CfgMgr
    return CfgMgr.ForwardTransportSvc(name,**kwargs)

def getZDCForwardTransportSvc(name="ForwardTransportSvc", **kwargs):
    print ("getZDCForwardTransportSvc")
    kwargs.setdefault("EndMarker",     141.580)
    kwargs.setdefault("TransportFlag", 0)
    kwargs.setdefault("EtaCut",        7.5)
    kwargs.setdefault("XiCut",         0)
    from AthenaCommon import CfgMgr
    return CfgMgr.ForwardTransportSvc(name,**kwargs)
