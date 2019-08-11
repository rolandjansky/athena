# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from TrigBjetHypo.TrigBjetHypoConf import TrigFarawayJetFinderAllTE

def getFarawayJetFinderAllTEInstance( cut ):
    return FarawayJetFinderAllTE( cut=cut, name="FarawayJetFinderAllTE_"+cut+"dR" )



class FarawayJetFinderAllTE (TrigFarawayJetFinderAllTE):
    __slots__ = []
    
    def __init__(self, cut, name):
        super( FarawayJetFinderAllTE, self ).__init__( name )
        
        self.MinDeltaRCut = float(cut)/10.0
        self.JetInputKey  = "SplitJet"
        self.JetOutputKey = "FarawayJet"
        

