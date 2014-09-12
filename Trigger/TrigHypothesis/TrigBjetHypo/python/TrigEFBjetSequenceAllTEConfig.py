# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigBjetHypo.TrigBjetHypoConf import TrigEFBjetSequenceAllTE

from AthenaCommon.SystemOfUnits import GeV


def getEFBjetAllTEInstance( ):
    return EFBjetSequenceAllTE( name="EFBjetSequenceAllTE" )


class EFBjetSequenceAllTE (TrigEFBjetSequenceAllTE):
    __slots__ = []
    
    def __init__(self, name):
        super( EFBjetSequenceAllTE, self ).__init__( name )
        
        self.JetInputKey  = "TrigJetRec"
        self.JetOutputKey = "TrigJetRec"
        self.UseSuperRoi  = False
        self.EtaHalfWidth = 0.2
        self.PhiHalfWidth = 0.2
