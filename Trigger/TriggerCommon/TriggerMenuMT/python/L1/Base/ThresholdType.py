# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from enum import Enum, unique
from functools import total_ordering

from AthenaCommon.Logging import logging
log = logging.getLogger(__name__)

@total_ordering
@unique
class ThrType( Enum ):

    # run 3 calo and muon thresholds
    # cTau is the combined taus computed in the L1Topo multiplicity board matching eTAU and jTAU
    eEM = 1; jEM = 2; eTAU = 3; jTAU = 4; cTAU=5; jJ = 6; jLJ = 7; gJ = 8; gLJ = 9; gXE = 10; gTE = 11; jXE = 12; jTE = 13; MU = 14 # noqa: E702

    # NIM thresholds
    BCM = 21; BCMCMB = 22; LUCID = 23; ZDC = 24; BPTX = 25; CALREQ = 26; MBTS = 27; MBTSSI = 28; NIM = 29 # noqa: E702

    # legacy calo thresholds
    EM = 41; TAU = 42; JET = 43; JE = 44; XE = 45; TE = 46; XS = 47 # noqa: E702

    # zero bias threshold on CTPIN
    ZB = 50 

    # topo thresholds
    TOPO = 60; MUTOPO = 61; MULTTOPO = 62; R2TOPO = 63 # noqa: E702

    # ALFA thresholds
    ALFA = 70

    @staticmethod
    def LegacyTypes():
        return [ ThrType.EM, ThrType.TAU, ThrType.JET, ThrType.XE, ThrType.TE, ThrType.XS, ThrType.ZB ]
    
    @staticmethod
    def Run3Types():
        return [ ThrType.MU, ThrType.eEM, ThrType.jEM, ThrType.eTAU, ThrType.jTAU, ThrType.cTAU, ThrType.jJ, ThrType.jLJ, ThrType.gJ, ThrType.gLJ, ThrType.gXE, ThrType.gTE, ThrType.jXE, ThrType.jTE ]
    
    @staticmethod
    def NIMTypes():
        return [ ThrType.BCM, ThrType.BCMCMB, ThrType.LUCID, ThrType.ZDC, ThrType.BPTX, ThrType.CALREQ, ThrType.MBTS, ThrType.MBTSSI, ThrType.NIM ]

    def __lt__(self,other):
        return self.name < other

    def __eq__(self,other):
        return self.name == other

    def __hash__(self):
        return hash(self.name)

    def __repr__(self):
        return self.name

    def __str__(self):
        return self.name
