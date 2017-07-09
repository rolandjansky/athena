# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


def getMETSigificanceAlg(suffix,jetSelection="Tier0",jetColl=""):
    from AthenaCommon import CfgMgr

    print "Generate METSignificance and METSignificanceAlg for METAssoc_"+suffix
    from AthenaCommon.AppMgr import ToolSvc
    from AthenaCommon.AlgSequence import AlgSequence
    topSequence = AlgSequence()

    doPFlow = 'PFlow' in suffix
    metSignificance = CfgMgr.met__METSignficance('METSignificance_'+suffix);

    ToolSvc += metSignificance

    if jetColl=="":
        jetColl = suffix+'Jets'
    signifAlg = CfgMgr.met__METSignificanceAlg('METSignificanceAlg_'+suffix,
                                               #METMapName='METAssoc_'+suffix,
                                               #METCoreName='MET_Core_'+suffix,
                                               #METName='MET_Reference_'+suffix,
                                               #InputJets=jetColl,
                                               Significance=metSignificance,
                                               #MuonSelectionTool=muonSel,
                                               #ElectronLHSelectionTool=elecSelLH,
                                               #PhotonIsEMSelectionTool=photonSelIsEM,
                                               #TauSelectionTool=tauSel,
                                       )
    return signifAlg
