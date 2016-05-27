# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


def getMETMakerAlg(metsuffix,jetPtCut=0,jetColl="",confsuffix=""):
    from AthenaCommon import CfgMgr

    print "Generate METMaker and METMakerAlg for METAssoc_"+metsuffix
    from AthenaCommon.AppMgr import ToolSvc
    from AthenaCommon.AlgSequence import AlgSequence
    topSequence = AlgSequence()

    doPFlow = 'PFlow' in metsuffix
    doTruth = metsuffix.startswith('Truth')
    metMaker = CfgMgr.met__METMaker('METMaker_'+metsuffix,
                                    DoPFlow=doPFlow,
                                    DoSoftTruth=doTruth,
                                    JetPtCut=jetPtCut);
    ToolSvc += metMaker

    muonSel = CfgMgr.CP__MuonSelectionTool("METMuonSelectionTool"+confsuffix,
                                           MuQuality=1, # Medium
                                           MaxEta=2.4)
    ToolSvc += muonSel

    elecSelLH = CfgMgr.AsgElectronLikelihoodTool("METEleSelLikelihood"+confsuffix,
                                                 ConfigFile="ElectronPhotonSelectorTools/offline/mc15_20150429/ElectronLikelihoodMediumOfflineConfig2015.conf")
    ToolSvc += elecSelLH

    from ROOT import egammaPID
    photonSelIsEM = CfgMgr.AsgPhotonIsEMSelector("METPhotonSelIsEM"+confsuffix,
                                                 isEMMask=egammaPID.PhotonTight,
                                                 ConfigFile="ElectronPhotonSelectorTools/offline/mc15_20150429/PhotonIsEMTightSelectorCutDefs.conf")
    ToolSvc += photonSelIsEM

    # tauSel = CfgMgr.TauAnalysisTools__TauSelectionTool("METTauSelectionTool"+confsuffix,
    #                                                    ConfigPath='TauAnalysisTools/00-01-05/Selection/recommended_selection_mc15.conf')
    # ToolSvc += tauSel

    if jetColl=="":
        jetColl = metsuffix+'Jets'
        if doTruth:
            jetColl = metsuffix.split('_')[1]+'Jets'
    makerAlg = CfgMgr.met__METMakerAlg('METMakerAlg_'+metsuffix+confsuffix,
                                       METMapName='METAssoc_'+metsuffix,
                                       METCoreName='MET_Core_'+metsuffix,
                                       METName='MET_Reference_'+metsuffix,
                                       InputJets=jetColl,
                                       Maker=metMaker,
                                       MuonSelectionTool=muonSel,
                                       ElectronLHSelectionTool=elecSelLH,
                                       PhotonIsEMSelectionTool=photonSelIsEM)
                                       # TauSelectionTool=tauSel)
    return makerAlg
