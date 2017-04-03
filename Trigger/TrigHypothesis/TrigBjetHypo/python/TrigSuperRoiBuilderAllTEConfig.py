# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigBjetHypo.TrigBjetHypoConf import TrigSuperRoiBuilderAllTE

from AthenaCommon.SystemOfUnits import GeV


def getSuperRoiBuilderAllTEInstance( ):
    return SuperRoiBuilderAllTE( name="SuperRoiBuilderAllTE" )


class SuperRoiBuilderAllTE (TrigSuperRoiBuilderAllTE):
    __slots__ = []
    
    def __init__(self, name):
        super( SuperRoiBuilderAllTE, self ).__init__( name )
        
        self.JetInputKey     = "TrigJetRec"
        self.JetOutputKey    = "SuperRoi"
        self.EtaHalfWidth    = 0.1
        self.PhiHalfWidth    = 0.1
        self.JetMinEt        = 30.0
        self.NJetsMax        = -1
        self.DynamicMinJetEt = False
        self.DynamicNJetsMax = 5
        self.DynamicEtFactor = 5

