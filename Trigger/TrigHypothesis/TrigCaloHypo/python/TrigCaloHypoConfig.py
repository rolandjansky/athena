# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


from AthenaCommon.SystemOfUnits import GeV
from TrigCaloHypo.TrigCaloHypoConf import TrigEFCaloHypoNoise
from LArCellRec.LArCellRecConf import LArNoisyROTool

class EFCaloHypoNoiseConfig (TrigEFCaloHypoNoise):
    __slots__ = []
    def __init__(self, name = "EFCaloHypoNoiseConfig",ef_thr=20*GeV,etaMin=0,etaMax=10):
        super( EFCaloHypoNoiseConfig, self ).__init__( name )

        self.Etcut = ef_thr
        self.BadFEBCut=3
        theLArNoisyROTool=LArNoisyROTool(SaturatedCellTightCut=20,MNBLooseCut=5,MNBTightCut=17)
        self.NoiseTool = theLArNoisyROTool
