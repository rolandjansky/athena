#  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
#

from AthenaCommon.Constants import VERBOSE,DEBUG,INFO,WARNING
from TrigEFMissingET.TrigEFMissingETMonitoring import *
from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import MenuSequence
from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import Chain, ChainStep

# from TrigUpgradeTest import TrigMissingETStepFromName

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


def TrigMissingETChainFromName(name):
    parts = name.split('_')
    # L1Seed = "L1_XE"+parts[-1][4:]
    L1Seed = "L1_XE50"
    parts=parts[1:-1]

    chainSteps = []

    # Implement this function
    def parseStepNames(name):
        return ["cell"]

    stepNames = parseStepNames(name)

    for n_metStep, s in enumerate(stepNames, start=1):
        stepName = "Step%d_%s" % (n_metStep, s)
        seq = []
        if s == "cell":
            seq.append(metCellSequence())

        chainSteps.append(ChainStep(stepName, seq))

    return Chain(name=name, Seed=L1Seed, ChainSteps=chainSteps)

