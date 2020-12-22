# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from __future__ import print_function
from AthenaConfiguration.ComponentFactory import CompFactory


def getMETMakerAlg(suffix,jetSelection="Tier0",jetColl="",doRetrieveORconstit=False, retainMuonConstit=False, doORMet=False, muonID=1, electronID="Medium", photonID="Tight"):

    print ("Generate METMaker and METMakerAlg for METAssoc_"+suffix)

    doPFlow = 'PFlow' in suffix
    doTruth = suffix.startswith('Truth')
    metMaker = CompFactory.getComp("met::METMaker")('METMaker_'+suffix,
                                    DoPFlow=doPFlow,
                                    DoSoftTruth=doTruth,
                                    JetSelection=jetSelection,
                                    )

    muonSel = CompFactory.getComp("CP::MuonSelectionTool")("MuonSelectionTool_METMakerAlg",
                                           MuQuality=muonID, # Medium
                                           MaxEta=2.4)

    elecSelLH = CompFactory.AsgElectronLikelihoodTool("EleSelLikelihood_METMakerAlg",
                                                 WorkingPoint=electronID+"LHElectron")

    photonSelIsEM = CompFactory.AsgPhotonIsEMSelector("PhotonSelIsEM_METMakerAlg",
                                                 WorkingPoint=photonID+"Photon")

    tauSel = CompFactory.getComp("TauAnalysisTools::TauSelectionTool")("TauSelectionTool_METMakerAlg")

    if jetColl=="":
        jetColl = suffix+'Jets'
        if doTruth:
            jetColl = suffix.split('_')[1]+'Jets'
    makerAlg = CompFactory.getComp("met::ORMETMakerAlg")('ORMETMakerAlg_'+suffix,
                                       METMapName='METAssoc_AntiKt4EMPFlow',
                                       ORMETMapName='METAssoc_'+suffix,
                                       METCoreName='MET_Core_'+suffix,
                                       METName='MET_Reference_'+suffix,
                                       InputJets=jetColl,
                                       Maker=metMaker,
                                       MuonSelectionTool=muonSel,
                                       ElectronLHSelectionTool=elecSelLH,
                                       PhotonIsEMSelectionTool=photonSelIsEM,
                                       TauSelectionTool=tauSel,
                                       DoRetrieveORconstit=doRetrieveORconstit,
                                       RetainMuonConstit=retainMuonConstit,
                                       DoORMet=doORMet,
                                       )
    return makerAlg

