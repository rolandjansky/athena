# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import MenuSequenceCA, ChainStep, Chain, SelectionCA, InEventRecoCA
from TrigCaloRec.TrigCaloRecConfig import hltCaloCellMakerCfg
from TrigEDMConfig.TriggerEDMRun3 import recordable
from AthenaConfiguration.ComponentFactory import CompFactory

def _cellsMET(flags, chainDict):
    recoAcc = InEventRecoCA('FastCaloMETReco')
    recoAcc.merge(hltCaloCellMakerCfg(flags, name="HLTCaloCellMaker_FS", roisKey="HLT_FSRoI"))
    from TrigEFMissingET.TrigEFMissingETMTConfig import getMETMonTool
    cellMETKey=recordable("HLT_MET_cell")
    recoAcc.addEventAlgo(CompFactory.HLT.MET.CellFex(MonTool=getMETMonTool(), METContainerKey=cellMETKey, CellName="CaloCellsFS"))

    selAcc=SelectionCA('FastCaloMET')
    selAcc.mergeReco(recoAcc)

    hypoAlg = CompFactory.TrigMissingETHypoAlg("METHypoAlg_Cells", METContainerKey=cellMETKey)
    selAcc.addHypoAlgo(hypoAlg)

    from TrigMissingETHypo.TrigMissingETHypoConfig import TrigMETCellHypoToolFromDict

    menuCellsSequence = MenuSequenceCA(selAcc,
                                      HypoToolGen=TrigMETCellHypoToolFromDict)                                      
    return ChainStep(name=selAcc.name, Sequences=[menuCellsSequence], chainDicts=[chainDict], multiplicity=[1])

def generateChains(flags, chainDict):
    thresholds = [p["L1threshold"] for p in chainDict['chainParts'] if p['signature'] == 'MET' ]
    chain = Chain( name=chainDict['chainName'], L1Thresholds=thresholds, ChainSteps=[ _cellsMET(flags, chainDict) ] )
    return chain