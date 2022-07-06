
# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from TrigAFPHypo.TrigAFPHypoConf import TrigAFPToFHypoAlg

class TrigAFPToFHypoAlgConfig( TrigAFPToFHypoAlg ):
    __slots__ = []
    def __init__(self, name = "TrigAFPToFHypoAlgConfig"):
        super( TrigAFPToFHypoAlgConfig, self ).__init__( name )

