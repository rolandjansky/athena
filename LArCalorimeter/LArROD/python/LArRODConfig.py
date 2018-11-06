# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import CfgMgr

def getLArRawChannelBuilder(name="LArRawChannelBuilder" , **kwargs):
    
    from AthenaCommon.AppMgr import ToolSvc

    kwargs.setdefault('LArRawChannelKey', "LArRawChannels")
    kwargs.setdefault('LArDigitKey', 'LArDigitContainer_MC')

    #Call required Cond-Algos:
    from LArRecUtils.LArAutoCorrTotalCondAlgDefault import  LArAutoCorrTotalCondAlgDefault
    from LArRecUtils.LArOFCCondAlgDefault import LArOFCCondAlgDefault
    from LArRecUtils.LArADC2MeVCondAlgDefault import LArADC2MeVCondAlgDefault

    #The order matters when running w/o MT
    LArADC2MeVCondAlgDefault()
    LArAutoCorrTotalCondAlgDefault()
    LArOFCCondAlgDefault()


    from LArROD.LArRODFlags import larRODFlags
    kwargs.setdefault('firstSample',larRODFlags.firstSample())
    
    return CfgMgr.LArRawChannelBuilderAlg(name, **kwargs)

def getLArRawChannelBuilder_DigiHSTruth(name="LArRawChannelBuilder_DigiHSTruth" , **kwargs):
    from AthenaCommon.AppMgr import ToolSvc

    kwargs.setdefault('LArRawChannelKey', "LArRawChannels_DigiHSTruth")
    kwargs.setdefault('LArDigitKey', 'LArDigitContainer_DigiHSTruth')

    #Call required Cond-Algos:
    from LArRecUtils.LArAutoCorrTotalCondAlgDefault import  LArAutoCorrTotalCondAlgDefault
    from LArRecUtils.LArOFCCondAlgDefault import LArOFCCondAlgDefault
    from LArRecUtils.LArADC2MeVCondAlgDefault import LArADC2MeVCondAlgDefault

    #The order matters when running w/o MT
    LArADC2MeVCondAlgDefault()
    LArAutoCorrTotalCondAlgDefault()
    LArOFCCondAlgDefault()


    from LArROD.LArRODFlags import larRODFlags
    kwargs.setdefault('firstSample',larRODFlags.firstSample())

    return CfgMgr.LArRawChannelBuilderAlg(name, **kwargs)
