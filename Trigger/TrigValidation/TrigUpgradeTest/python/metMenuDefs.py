#  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
#

from AthenaCommon.Constants import VERBOSE,DEBUG,INFO,WARNING
from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import MenuSequence
from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import Chain, ChainStep 

def metCellSequence():
    # menu components   
    from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import MenuSequence
    from AthenaCommon.CFElements import parOR, seqAND, seqOR, stepSeq

    #input maker
    from DecisionHandling.DecisionHandlingConf import InputMakerForRoI
    InputMakerAlg = InputMakerForRoI("MetCellInputMaker", OutputLevel = DEBUG, RoIsLink="initialRoI")
    InputMakerAlg.RoIs='METCellRoI'
    
    #reco sequence
    from TrigUpgradeTest.metDefs import metCellRecoSequence
    recoSequence, sequenceOut = metCellRecoSequence(InputMakerAlg.RoIs)

    #hypo
    from TrigMissingETHypo.TrigMissingETHypoConfigMT import MissingETHypoAlgMT, TrigMETCellHypoToolFromName, MissingETHypoToolMT

    metHypoAlg = MissingETHypoAlgMT("METHypoAlg")
    metHypoAlg.METContainerKey=sequenceOut
    metHypoAlg.OutputLevel=DEBUG

    # sequence
    metCellSequence = seqAND("metCellSequence",[InputMakerAlg, recoSequence])

    return  MenuSequence( Sequence    = metCellSequence,
                          Maker       = InputMakerAlg,
                          Hypo        = metHypoAlg,
                          HypoToolGen = TrigMETCellHypoToolFromName )

