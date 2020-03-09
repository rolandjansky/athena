# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from TrigEgammaHypo.TrigEgammaHypoConf import TrigEFMtAllTE
from AthenaCommon.SystemOfUnits import GeV

class TrigEFMtAllTE_Generic(TrigEFMtAllTE):
    def __init__(self, name='TrigEFMtAllTE_Generic', minMtCut = 20*GeV, maxNbElectrons = 10, minElectronEt = 10*GeV):
        super(TrigEFMtAllTE_Generic, self).__init__(name)
        self.MinMtCut       = minMtCut
        self.MaxNbElectrons = maxNbElectrons
        self.MinElectronEt  = minElectronEt
