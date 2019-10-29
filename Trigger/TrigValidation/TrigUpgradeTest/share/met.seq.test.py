#
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#

include("TrigUpgradeTest/testHLT_MT.py")

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()


from DecisionHandling.DecisionHandlingConf import InputMakerForRoI
InputMakerAlg = InputMakerForRoI("MetCellInputMaker", RoIsLink="initialRoI")
InputMakerAlg.RoIs='METCellRoI'
InputMakerAlg.InputMakerInputDecisions=["HLTNav_L1MET"]
InputMakerAlg.InputMakerOutputDecisions=["InputMaker_from_L1MET"]
topSequence += InputMakerAlg

from TriggerMenuMT.HLTMenuConfig.MET.METSequences import metCellRecoSequence
cellRecoSeq, METkey = metCellRecoSequence(InputMakerAlg.RoIs)
topSequence += cellRecoSeq

from TrigMissingETHypo.TrigMissingETHypoConfigMT import MissingETHypoAlgMT, MissingETHypoToolMT

metHypoTool = MissingETHypoToolMT("METHypoTool")
metHypoTool.metThreshold=50

metHypoAlg = MissingETHypoAlgMT("METHypoAlg")
metHypoAlg.HypoTools = [metHypoTool]
metHypoAlg.METContainerKey="HLT_MET"
metHypoAlg.HypoInputDecisions = InputMakerAlg.InputMakerOutputDecisions[0]
metHypoAlg.HypoOutputDecisions = "EFMETDecisions"

topSequence += metHypoAlg


topSequence.L1Decoder.Chains="HLTChains"
