# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigMissingETHypo.TrigMissingETHypoConf import (
        HLT__MET__METHypo,
        HLT__MET__TEHypo,
        HLT__MET__XSHypo)
from TrigMissingETHypo.TrigMissingETHypoMonitoring import (
        hXS, hXS2)
from TrigEFMissingET.TrigEFMissingETMonitoring import add_monitor

from AthenaCommon.SystemOfUnits import GeV

class METHypo(HLT__MET__METHypo):
    __slots__ = []
    def __init__(self, name, **kwargs):
        super(METHypo, self).__init__(name, **kwargs)
        add_monitor(self, "Validation", ["standard"])

class TEHypo(HLT__MET__TEHypo):
    __slots__ = []
    def __init__(self, name, **kwargs):
        super(TEHypo, self).__init__(name, **kwargs)
        add_monitor(self, "Validation", ["standard"])

class XSHypo(HLT__MET__XSHypo):
    __slots__ = []
    def __init__(self, name, **kwargs):
        super(XSHypo, self).__init__(name, **kwargs)
        add_monitor(self, "Validation", ["standard", hXS, hXS2])
