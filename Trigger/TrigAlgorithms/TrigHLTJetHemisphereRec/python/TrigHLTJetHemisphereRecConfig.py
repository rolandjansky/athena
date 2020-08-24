# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.SystemOfUnits import GeV
from TrigHLTJetHemisphereRec.TrigHLTJetHemisphereRecConf import TrigHLTJetHemisphereRec

class TrigHLTJetHemisphereRec_Builder(TrigHLTJetHemisphereRec):
    """Supply a kinematic cuts for the preselection going into hemisphere reco"""
    def __init__(self,
                 name="TrigHLTJetHemisphereRec",
                 jetPtCut=30.0 * GeV,
                 jetEtaCut=3.2 ):
        TrigHLTJetHemisphereRec.__init__(self, name)

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("TrigHLTJetHemisphereRec_Time")

        self.AthenaMonTools = [ time ]
