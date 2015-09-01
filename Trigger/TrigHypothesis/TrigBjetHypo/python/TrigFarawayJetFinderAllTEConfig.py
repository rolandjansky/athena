# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigBjetHypo.TrigBjetHypoConf import TrigFarawayJetFinderAllTE

from AthenaCommon.Logging import logging
from AthenaCommon.SystemOfUnits import GeV


def getFarawayJetFinderAllTEInstance( cut ):
    return FarawayJetFinderAllTE( cut=cut, name="FarawayJetFinderAllTE_"+cut+"dR" )



class FarawayJetFinderAllTE (TrigFarawayJetFinderAllTE):
    __slots__ = []
    
    def __init__(self, cut, name):
        super( FarawayJetFinderAllTE, self ).__init__( name )
        
        mlog = logging.getLogger('BjetHypoConfig.py')

        self.MinDeltaRCut = float(cut)/10.0
        self.JetInputKey  = "SplitJet"
        self.JetOutputKey = "FarawayJet"
        

