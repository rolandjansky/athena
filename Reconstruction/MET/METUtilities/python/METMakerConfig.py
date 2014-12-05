# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


def getMETMakerAlg(suffix):
    from AthenaCommon import CfgMgr

    print "Generate METMaker and METMakerAlg for METAssoc_"+suffix
    from AthenaCommon.AppMgr import ToolSvc
    from AthenaCommon.AlgSequence import AlgSequence
    topSequence = AlgSequence()

    metMaker = CfgMgr.met__METMaker('METMaker_'+suffix,
                                    JetPtCut=0e3,
                                    DoJetJvfCut=False);
    ToolSvc += metMaker

    makerAlg = CfgMgr.met__METMakerAlg('METMakerAlg_'+suffix,
                                       METMapName='METAssoc_'+suffix,
                                       METCoreName='MET_Core_'+suffix,
                                       METName='MET_Reference_'+suffix,
                                       Maker=metMaker)
    return makerAlg
