# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigBjetHypo.TrigBjetHypoConf import TrigLeptonJetFex



class L2LeptonJetFex (TrigLeptonJetFex):
    __slots__ = []
    def __init__(self, name = "L2LeptonJetFex"):
        super( L2LeptonJetFex, self ).__init__( name )

        self.Instance = "L2"



class EFLeptonJetFex (TrigLeptonJetFex):
    __slots__ = []
    def __init__(self, name = "EFLeptonJetFex"):
        super( EFLeptonJetFex, self ).__init__( name )

        self.Instance = "EF"


