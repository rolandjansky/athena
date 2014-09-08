# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigEgammaHypo.TrigEgammaHypoConf import *
from AthenaCommon.SystemOfUnits import GeV

class TrigEFHadronicRecoil_recoil40(TrigEFHadronicRecoil):
    def __init__(self, name='TrigEFHadronicRecoil_recoil25'):
        super(TrigEFHadronicRecoil_recoil40, self).__init__(name)
        self.RecoilCut = 40.0*GeV
        
