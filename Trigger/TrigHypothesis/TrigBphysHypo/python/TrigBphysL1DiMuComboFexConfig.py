# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigBphysHypo.TrigBphysHypoConf import TrigBphysL1DiMuComboFex
from TrigBphysHypo.TrigBphysHypoConf import TrigBphysL1DiMuComboFexValMonitoring



class TrigBphysL1DiMuComboFexConfig( TrigBphysL1DiMuComboFex ):

    __slots__ = []
    def __init__ (self, name="TrigBphysL1DiMuComboFexConfig"):

        super(TrigBphysL1DiMuComboFexConfig, self).__init__(name)

        self.AthenaMonTools += [ TrigBphysL1DiMuComboFexValMonitoring() ]
