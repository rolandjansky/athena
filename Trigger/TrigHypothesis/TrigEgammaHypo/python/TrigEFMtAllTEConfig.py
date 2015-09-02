# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigEgammaHypo.TrigEgammaHypoConf import *
from AthenaCommon.SystemOfUnits import GeV
from TrigEgammaHypo.TrigEFMtAllTEMonitoring import *

class TrigEFMtAllTE_Generic(TrigEFMtAllTE):
    def __init__(self, name='TrigEFMtAllTE_Generic', minMtCut = 20*GeV, maxNbElectrons = 10, minElectronEt = 10*GeV):
        super(TrigEFMtAllTE_Generic, self).__init__(name)
        self.MinMtCut       = minMtCut
        self.MaxNbElectrons = maxNbElectrons
        self.MinElectronEt  = minElectronEt
