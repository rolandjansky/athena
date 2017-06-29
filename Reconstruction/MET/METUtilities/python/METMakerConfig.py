# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


def getMETMakerAlg(suffix,jetSelection="Tier0",jetColl=""):
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
                                    JetSelection=jetSelection,
                                    );
    ToolSvc += metMaker

    muonSel = CfgMgr.CP__MuonSelectionTool("MuonSelectionTool_METMakerAlg",
                                           MuQuality=1, # Medium
                                           MaxEta=2.4)
    ToolSvc += muonSel

    elecSelLH = CfgMgr.AsgElectronLikelihoodTool("EleSelLikelihood_METMakerAlg",
                                                 WorkingPoint="MediumLHElectron")
    ToolSvc += elecSelLH

    photonSelIsEM = CfgMgr.AsgPhotonIsEMSelector("PhotonSelIsEM_METMakerAlg",
                                                 WorkingPoint="TightPhoton")
    ToolSvc += photonSelIsEM

    tauSel = CfgMgr.TauAnalysisTools__TauSelectionTool("TauSelectionTool_METMakerAlg")
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
