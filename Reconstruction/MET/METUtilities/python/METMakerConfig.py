# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from __future__ import print_function
from AthenaConfiguration.ComponentFactory import CompFactory


def getMETMakerAlg(suffix,jetSelection="Tier0",jetColl=""):

    print ("Generate METMaker and METMakerAlg for METAssoc_"+suffix)

    doPFlow = 'PFlow' in suffix
    doTruth = suffix.startswith('Truth')
    metMaker = CompFactory.getComp("met::METMaker")('METMaker_'+suffix,
                                    DoPFlow=doPFlow,
                                    DoSoftTruth=doTruth,
                                    JetSelection=jetSelection,
                                    )

    muonSel = CompFactory.getComp("CP::MuonSelectionTool")("MuonSelectionTool_METMakerAlg",
                                           MuQuality=1, # Medium
                                           MaxEta=2.4)

    elecSelLH = CompFactory.AsgElectronLikelihoodTool("EleSelLikelihood_METMakerAlg",
                                                 WorkingPoint="MediumLHElectron")

    photonSelIsEM = CompFactory.AsgPhotonIsEMSelector("PhotonSelIsEM_METMakerAlg",
                                                 WorkingPoint="TightPhoton")

    from ROOT import TauAnalysisTools
    SelectionCuts = TauAnalysisTools.SelectionCuts

    tauSel = CompFactory.getComp("TauAnalysisTools::TauSelectionTool")("TauSelectionTool_METMakerAlg",
                                                                       ConfigPath = "",
                                                                       SelectionCuts = int(SelectionCuts.CutPt | SelectionCuts.CutAbsEta | SelectionCuts.CutAbsCharge | SelectionCuts.CutNTrack),
                                                                       PtMin = 20.0,
                                                                       JetIDWP = TauAnalysisTools.JETIDNONE,
                                                                       EleOLR = False,
                                                                       NTracks = (0, 1, 2, 3, 4, 5),
                                                                       AbsCharges = (0, 1, 2, 3),
                                                                       AbsEtaRegion = (0.0, 1.37, 1.52, 2.5))

    if jetColl=="":
        jetColl = suffix+'Jets'
        if doTruth:
            jetColl = suffix.split('_')[1]+'Jets'
    makerAlg = CompFactory.getComp("met::METMakerAlg")('METMakerAlg_'+suffix,
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
