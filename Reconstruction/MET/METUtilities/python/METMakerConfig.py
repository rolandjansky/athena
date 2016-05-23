# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


def getMETMakerAlg(suffix,jetPtCut=0,jetColl=""):
    from AthenaCommon import CfgMgr

    print "Generate METMaker and METMakerAlg for METAssoc_"+suffix
    from AthenaCommon.AppMgr import ToolSvc
    from AthenaCommon.AlgSequence import AlgSequence
    topSequence = AlgSequence()

    doPFlow = 'PFlow' in suffix
    doTruth = suffix.startswith('Truth')
    metMaker = CfgMgr.met__METMaker('METMaker_'+suffix,
                                    DoPFlow=doPFlow,
                                    DoSoftTruth=doTruth,
                                    JetSelection='Tier0',
                                    );
    ToolSvc += metMaker

    muonSel = CfgMgr.CP__MuonSelectionTool("MuonSelectionTool",
                                           MuQuality=1, # Medium
                                           MaxEta=2.4)
    ToolSvc += muonSel

    elecSelLH = CfgMgr.AsgElectronLikelihoodTool("EleSelLikelihood",
                                                 ConfigFile="ElectronPhotonSelectorTools/offline/mc15_20150429/ElectronLikelihoodMediumOfflineConfig2015.conf")
    ToolSvc += elecSelLH

    from ROOT import egammaPID
    photonSelIsEM = CfgMgr.AsgPhotonIsEMSelector("PhotonSelIsEM",
                                                 isEMMask=egammaPID.PhotonTight,
                                                 ConfigFile="ElectronPhotonSelectorTools/offline/mc15_20150429/PhotonIsEMTightSelectorCutDefs.conf")
    ToolSvc += photonSelIsEM

    tauSel = CfgMgr.TauAnalysisTools__TauSelectionTool("TauSelectionTool")
    ToolSvc += tauSel

    if jetColl=="":
        jetColl = suffix+'Jets'
        if doTruth:
            jetColl = suffix.split('_')[1]+'Jets'
    makerAlg = CfgMgr.met__METMakerAlg('METMakerAlg_'+suffix,
                                       METMapName='METAssoc_'+suffix,
                                       METCoreName='MET_Core_'+suffix,
                                       METName='MET_Reference_'+suffix,
                                       InputJets=jetColl,
                                       Maker=metMaker,
                                       MuonSelectionTool=muonSel,
                                       ElectronLHSelectionTool=elecSelLH,
                                       PhotonIsEMSelectionTool=photonSelIsEM,
                                       TauSelectionTool=tauSel,
                                       )
    return makerAlg
