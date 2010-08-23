# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigPileUpHypo.TrigPileUpHypoConf import TrigPileUpHypo
from AthenaCommon.SystemOfUnits import GeV


class TrigPileUpHypoBase(TrigPileUpHypo):
    __slots__ = []
    def __init__(self, name):
        super( TrigPileUpHypoBase, self ).__init__( name )


        #from TrigPileUpHypo.TrigPileUpHypoMonitoring import TrigPileUpHypoOnlineMonitoring
        #Online = TrigPileUpHypoOnlineMonitoring()

        #self.AthenaMonTools = [ Online ]


#TrigPileUpHypoBase PileUpFJ( "PileUpFJ" )
