# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


def getMETSigificanceAlg(suffix,jetSelection="Tier0",jetColl=""):
    from AthenaCommon import CfgMgr

    print "Generate METSignificance and METSignificanceAlg for METAssoc_"+suffix
    from AthenaCommon.AppMgr import ToolSvc
    from AthenaCommon.AlgSequence import AlgSequence
    topSequence = AlgSequence()

    doPFlow = 'PFlow' in suffix
    doTruth = suffix.startswith('Truth')
    metSignificance = CfgMgr.met__METSignficance('METSignificance_'+suffix);

    ToolSvc += metSignificance

    #muonSel = CfgMgr.CP__MuonSelectionTool("MuonSelectionTool_METSignificanceAlg",
    #                                       MuQuality=1, # Medium
    #                                       MaxEta=2.4)
    #ToolSvc += muonSel
    #
    #elecSelLH = CfgMgr.AsgElectronLikelihoodTool("EleSelLikelihood_METSignificanceAlg",
    #                                             WorkingPoint="MediumLHElectron")
    #ToolSvc += elecSelLH
    #
    #photonSelIsEM = CfgMgr.AsgPhotonIsEMSelector("PhotonSelIsEM_METMakerAlg",
    #                                             WorkingPoint="TightPhoton")
    #ToolSvc += photonSelIsEM
    #
    #tauSel = CfgMgr.TauAnalysisTools__TauSelectionTool("TauSelectionTool_METMakerAlg")
    #ToolSvc += tauSel

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
