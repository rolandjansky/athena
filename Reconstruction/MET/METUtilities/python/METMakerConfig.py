# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from __future__ import print_function
from AthenaConfiguration.ComponentFactory import CompFactory

def getMETMaker(name="METMaker", **kwargs):
    return CompFactory.getComp("met::METMaker")(name, **kwargs)

def getMuonSelectionTool(name="MuonSelectionTool_METMakerAlg"):
    muonSel = CompFactory.getComp("CP::MuonSelectionTool")(name,
                                                           MuQuality=1, # Medium
                                                           TurnOffMomCorr= True,
                                                           MaxEta=2.4,
                                                           AllowSettingGeometryOnTheFly = True)
    return muonSel

def getEleSelLikelihood(name="EleSelLikelihood_METMakerAlg"):
    elecSelLH = CompFactory.AsgElectronLikelihoodTool(name,
                                                      WorkingPoint="MediumLHElectron")
    return elecSelLH

def getPhotonSelIsEM(name="PhotonSelIsEM_METMakerAlg"):
    photonSelIsEM = CompFactory.AsgPhotonIsEMSelector(name,
                                                      WorkingPoint="TightPhoton")
    return photonSelIsEM

def getTauSelectionTool(name="TauSelectionTool_METMakerAlg"):
    from ROOT import TauAnalysisTools
    SelectionCuts = TauAnalysisTools.SelectionCuts

    tauSel = CompFactory.getComp("TauAnalysisTools::TauSelectionTool")("TauSelectionTool_METMakerAlg",
                                                                       ConfigPath = "",
                                                                       SelectionCuts = int(SelectionCuts.CutPt | SelectionCuts.CutAbsEta | SelectionCuts.CutAbsCharge | SelectionCuts.CutNTrack | SelectionCuts.CutJetIDWP),
                                                                       PtMin = 20.0,
                                                                       JetIDWP = TauAnalysisTools.JETIDRNNMEDIUM,
                                                                       NTracks = (1, 3),
                                                                       AbsCharge = 1,
                                                                       AbsEtaRegion = (0.0, 1.37, 1.52, 2.5))
    return tauSel

def getMETMakerAlg(suffix,jetSelection="Tier0",jetColl=""):

    print ("Generate METMaker and METMakerAlg for METAssoc_"+suffix)

    doPFlow = 'PFlow' in suffix
    doTruth = suffix.startswith('Truth')
    metMaker = getMETMaker('METMaker_'+suffix,
                           DoPFlow=doPFlow,
                           DoSoftTruth=doTruth,
                           JetSelection=jetSelection)

    muonSel = getMuonSelectionTool()

    elecSelLH = getEleSelLikelihood()

    photonSelIsEM = getPhotonSelIsEM()

    tauSel = getTauSelectionTool()

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
